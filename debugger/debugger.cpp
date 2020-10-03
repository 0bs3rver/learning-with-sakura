#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>

#define BKPT   0xCC
#define BKPT_MASK   0xFFFFFFFFFFFFFF00

#define FILEPATH_LEN 100
#define SYMBOL_NAME_LEN 30
#define BUF_LEN 200

char filename[FILEPATH_LEN+1];
FILE* fp;
int child_pid;

typedef struct
{
    long addr;
    long original_code;
    char name[SYMBOL_NAME_LEN+1];
} Breakpoint;

Breakpoint* breakpoints;
int bp_count;

void parse_elf_file()
{
    Elf64_Ehdr elf_header;
    Elf64_Shdr section_header;
    fp = fopen(filename, "r");
    if(!fp)
    {
        printf("Failed to open ELF file!\n");
        exit(-1);
    }

    fread(&elf_header, 1, sizeof(elf_header), fp);  // read elf header

    fseek(fp, elf_header.e_shoff, SEEK_SET);    // skip to section headers

    for(int i = 0; i < elf_header.e_shnum; ++i) // iterate through headers
    {
        fread(&section_header, 1, sizeof(section_header), fp);  // read section header
        if(section_header.sh_type == SHT_SYMTAB)    // check if this section is symbol table
        {

            Elf64_Shdr strtab_header;   // we need to get strtab associated with this symtab to get functions names
            long strtab_hdr_offset = elf_header.e_shoff+section_header.sh_link*sizeof(section_header); // calculate offset to strtab header
            fseek(fp, strtab_hdr_offset, SEEK_SET);//移动文件指针
            fread(&strtab_header, 1, sizeof(strtab_header), fp);    // read strtab header
            fseek(fp, section_header.sh_offset, SEEK_SET);

            int entries = section_header.sh_size / section_header.sh_entsize;
            //printf("Found symtab with %d entries\n", entries);
            breakpoints = static_cast<Breakpoint *>(malloc(entries * sizeof(Breakpoint)));   // there are more entries than just functions

            for(i = 0; i < entries; ++i)
            {
                Elf64_Sym symbol;
                fread(&symbol, 1, sizeof(symbol), fp);          // read symbol
                if(ELF64_ST_TYPE(symbol.st_info) == STT_FUNC    // symbol is a function
                   && symbol.st_name != 0                      // symbol has name
                   && symbol.st_value != 0)                    // symbol has address within binary
                {
                    //printf("Found function at offset %lx", symbol.st_value);

                    long pos = ftell(fp); //得到当前文件指针相对于文件首的偏移
                    fseek(fp, strtab_header.sh_offset+symbol.st_name, SEEK_SET);
                    //char symbol_name[SYMBOL_NAME_LEN+1];
                    //fread(symbol_name, SYMBOL_NAME_LEN, sizeof(char), fp);
                    //printf(" %s", symbol_name);

                    breakpoints[bp_count].addr = symbol.st_value;   // get address to beginning of function
                    fread(breakpoints[bp_count].name, SYMBOL_NAME_LEN, sizeof(char), fp);   // get function name
                    //printf("BP at %lx\n", breakpoints[bp_count].addr);

                    fseek(fp, pos, SEEK_SET);
                    bp_count++;

                    //printf("\n");
                }
            }
        }
    }
}

void insert_brakepoints()//将每个函数的第一个指令更改为断点
{
    for(int i = 0; i < bp_count; ++i)
    {
        breakpoints[i].original_code = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoints[i].addr, 0);
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[i].addr, (breakpoints[i].original_code & BKPT_MASK) | BKPT); // insert breakpoint
    }
}

void prepare_breakpoints()
{
    parse_elf_file();//解析elf文件
    insert_brakepoints();//插入断点
}

int get_bp_id(long addr)
{
    for(int i = 0; i < bp_count; ++i)
    {
        if(breakpoints[i].addr == addr)
            return i;
    }
    return -1;
}

void trace()
{
    int status;
    ptrace(PTRACE_CONT, child_pid, 0, 0);   // start child process
    printf("Tracing started\n===\n\n");
    while(1)
    {
        waitpid(child_pid, &status, 0);     // wait for change of status

        if(WIFEXITED(status))
        {
            printf("Child finished\n");
            return;
        }

        if(WIFSTOPPED(status))  // child stopped
        {
            if(WSTOPSIG(status) == SIGTRAP) // child stopped on sigtrap
            {
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
                int id = get_bp_id(regs.rip-1); // -1 because rip is now set to next inst and BP is 1 byte
                if(id == -1)
                {
                    printf("Unexpected SIGTRAP %llx\n", regs.rip);
                    return;
                }
                else
                {
                    printf("%s();\n", breakpoints[id].name);
                    regs.rip = breakpoints[id].addr;
                    ptrace(PTRACE_SETREGS, child_pid, 0, &regs);    // set rip back to good position
                    ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[id].addr, breakpoints[id].original_code); // return original instruction
                    ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0);     // step instruction
                    wait(NULL); // wait for singlestep
                    ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[id].addr, (breakpoints[id].original_code & BKPT_MASK) | BKPT); // insert breakpoint again
                }
            }
            if(((status >> 16) & 0xffff) == PTRACE_EVENT_EXIT)
            {
                printf("Child finished\n");
                return;
            }
        }
        ptrace(PTRACE_CONT, child_pid, 0, 0);   // continue child process
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Usage: tracer path\n");
        return -1;
    }
    strncpy(filename, argv[1], FILEPATH_LEN);
    child_pid = fork();
    if(child_pid == 0)  // child process - tracee
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);//请求调用ptrace并执行我们要跟踪的程序
        printf("Failed to execl!!\n");
        exit(-1);
    }
    else    // parent - tracer
    {
        printf("PID %d\n", child_pid);
        wait(NULL);
        prepare_breakpoints();
        trace();
        free(breakpoints);
    }
    return 0;
}
