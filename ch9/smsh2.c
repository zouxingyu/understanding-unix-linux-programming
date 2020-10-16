#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PROMPT "$"
#define BUFFERSIZE 32
#define ARGSIZE 32
#define MAXVARSIZE 100
typedef enum Status { NEUTRAL, CONDITION, THEN, ELSE } Status;
typedef enum Result { SUCCESS, FAIL } Result;

Status ifStatus = NEUTRAL;
Result ifResult = SUCCESS;

typedef enum Scope { GLOBAL, LOCAL } Scope;
typedef struct Variable {
    char *str;
    Scope scope;
} Variable;
int varTableSize;
Variable varTable[MAXVARSIZE];
extern char **environ;

char *GetString(char *start, int len) {
    char *ptr = malloc(len + 1);
    if (!ptr) return NULL;
    memcpy(ptr, start, len);
    ptr[len] = '\0';
    return ptr;
}
bool IsValidAssignStr(char *str) {
    if(!str || !*str) return false;
    if (isdigit(*str++)) return false;
    while (*str) {
        if (!(isalnum(*str)) || *str == '_') {
            return false;
        }
        ++str;
    }
    return true;
}
int VEnviron2Table(char *env[]) {
    varTableSize = 0;
    while (*env) {
        if(varTableSize == MAXVARSIZE)return 0;
        Variable *var = &varTable[varTableSize];
        char *tmp =  GetString(*env, strlen(*env));
        if(tmp) var->str = tmp;
        else return 0;
        ++varTableSize;
        ++env;
    }
    return 1;
}
char **VTable2Environ() {
    char **env = malloc(sizeof(Variable) * (varTableSize + 1));
    if (!env) return NULL;
    int i, j;
    for (i = 0, j = 0; i < varTableSize; ++i) {
        if (varTable[i].scope == GLOBAL) {
            env[j++] = varTable[i].str;
        }
    }
    env[j] = NULL;
    return env;
}
void VList() {
    for (int i = 0; i < varTableSize; ++i) {
        if (varTable[i].scope == GLOBAL) printf("*");
        printf("%s\n", varTable[i].str);
    }
}

Variable *FindVariable(char *name, bool returnEmpry) {
    int len = strlen(name);
    for (int i = 0; i < varTableSize; ++i) {
        if (!strncmp(varTable[i].str, name, len) &&
            varTable[i].str[len] == '=') {
            return &varTable[i];
        }
    }
    if (varTableSize != MAXVARSIZE && returnEmpry) {
        return &varTable[varTableSize];
    } else {
        return NULL;
    }
}
char *Concat(char *name, char *value) {
    char *ptr = malloc(strlen(name) + strlen(value) + 2);
    if (ptr) {
        sprintf(ptr, "%s=%s", name, value);
        return ptr;
    } else {
        return NULL;
    }
}
char *VLookUp(char *name) {
    Variable *var;
    if (var = FindVariable(name, false)) {
        return var->str + 1 + strlen(name);
    } else {
        return NULL;
    }
}
int Vexport(char *str) {
    if (!IsValidAssignStr(str)) return 1;
    Variable *var;
    if (var = FindVariable(str, true)) {
        if (var->str) {
            var->scope = GLOBAL;
        } else {
            char *ptr = Concat(str, "");
            if (ptr) {
                var->str = ptr;
                var->scope = GLOBAL;
                ++varTableSize;
            } else {
                return 1;
            }
        }
        return 0;
    } else {
        return 1;
    }
}
int VStore(char *name, char *value) {
    Variable *var;
    char *str;
    if ((var = FindVariable(name, true)) && (str = Concat(name, value))) {
        if (var->str) {
            free(var->str);
        }else{
            ++varTableSize;
        }
        var->str = str;
        var->scope = LOCAL;
        return 0;
    }
    return 1;
}

int Assign(char *str) {
    int ret;
    char *tmp = str;
    while (*tmp) {
        if (*tmp == '=') {
            *tmp = '\0';
            break;
        }
        ++tmp;
    }
    ret = (IsValidAssignStr(str) ? VStore(str, tmp + 1) : -1);
    *tmp = '=';
    return ret;
}
int lastStat = 0;
void Fatal(char *str, int n) {
    fprintf(stderr, "ERROR: %s\n", str);
    exit(n);
}
void *Emalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        Fatal("malloc failed", 2);
    }
    return ptr;
}
void *Erealloc(void *ptr, size_t size) {
    void *nptr = realloc(ptr, size);
    if (ptr == NULL) {
        Fatal("realloc failed", 2);
    }
    return nptr;
}
char *GetCommand(FILE *input, int *lenPtr) {
    int c;
    int len = 0;
    int bufSize = 0;
    char *buf;
    printf("%s", PROMPT);
    while ((c = getc(input)) != EOF) {
        if (!bufSize) {
            buf = Emalloc(BUFFERSIZE);
            bufSize = BUFFERSIZE;
        } else if (len + 1 >= bufSize) {
            buf = Erealloc(buf, bufSize + BUFFERSIZE);
            bufSize += BUFFERSIZE;
        }
        buf[len++] = c;
        if (c == '\n') break;
    }
    if (c == EOF) return NULL;
    buf[len] = '\0';
    *lenPtr = len;
    return buf;
}
char **GetArgList(char *command, int commandLen) {
    if (!command || *command == '\0') {
        return NULL;
    }
    char **argList;
    int argListSize = 0;
    int argListPos = 0;
    char *start = command;
    char *end = command + commandLen;
    // printf("%s, %d\n", command, commandLen);
    // remvoe leading and trailing whitespace
    if (start < end && isspace(*start)) ++start;
    if (start == end) return NULL;
    bool inSpace = true;
    int len;
    char *pos = start;
    int spots = 0;
    while (start < end) {
        if (isspace(*start)) {
            if (!inSpace) {
                inSpace = true;
                if (!argListPos) {
                    argList = Emalloc(ARGSIZE);
                    spots += ARGSIZE / sizeof(char *);
                    argListSize += ARGSIZE;
                } else if (argListPos + 1 >= spots) {
                    argList = Erealloc(argList, argListSize + ARGSIZE);
                    spots += ARGSIZE / sizeof(char *);
                    argListSize += ARGSIZE;
                }
                char *ptr = GetString(pos, len);
                if (!ptr) return NULL;
                argList[argListPos++] = ptr;
                //        printf("\n%d, %s\n", argListPos - 1,
                //        argList[argListPos - 1]);
            }
        } else if (inSpace) {
            len = 1;
            inSpace = false;
            pos = start;
        } else {
            ++len;
        }
        ++start;
    }
    if (!argListPos) return NULL;
    argList[argListPos] = NULL;
    return argList;
}
int Execute(char *argList[]) {
    int pid;
    int wstatus;
    int ret;
    pid = fork();
    if (!pid) {
        environ = VTable2Environ();
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        execvp(argList[0], argList);
        perror("execvp");
        exit(1);
    } else if (pid != -1) {
        ret = wait(&wstatus);
        if (ret == -1) {
            perror("wait");
            return -1;
        } else {
            return wstatus;
        }
    } else {
        perror("fork");
        return -1;
    }
}
bool BuildInCommand(char *argList[], int *ret) {
    if (!strcmp(*argList, "set")) {
        VList();
        *ret = 0;
        return true;
    } else if (strchr(*argList, '=')) {
        *ret = Assign(*argList);
        if (*ret != -1) {
            return true;
        }else{
            return false;
        }
    } else if (!strcmp(*argList, "export")) {
        *ret = Vexport(argList[1]);
        return true;
    } else {
        return false;
    }
}
int SysError(char *str) {
    ifStatus = NEUTRAL;
    fprintf(stderr, "syntax error:%s\n", str);
    return -1;
}
bool IsControlCommand(char *arg) {
    if (!strcmp(arg, "if") || !strcmp(arg, "then") || !strcmp(arg, "else") ||
        !strcmp(arg, "fi"))
        return true;
    else
        return false;
}
bool OkToExecute() {
    if (ifStatus == CONDITION) {
        SysError("then expected");
        return false;
    } else if (ifStatus == THEN) {
        return ifResult == SUCCESS ? true : false;
    } else if (ifStatus == ELSE) {
        return ifResult == FAIL ? true : false;
    } else {
        return true;
    }
}
int DoControlCommand(char *argList[]) {
    int Process(char *argList[]);
    char *keyword = *argList;
    if (!keyword) return 0;
    int ret = -1;
    if (!strcmp(keyword, "if")) {
        if (ifStatus != NEUTRAL) {
            ret = SysError("if unexpected");
        } else {
            lastStat = Process(argList + 1);
            ifResult = (lastStat == 0 ? SUCCESS : FAIL);
            ifStatus = CONDITION;
            ret = 0;
        }
    } else if (!strcmp(keyword, "then")) {
        if (ifStatus != CONDITION) {
            ret = SysError("then unexpected");
        } else {
            ifStatus = THEN;
            ret = 0;
        }
    } else if (!strcmp(keyword, "else")) {
        if (ifStatus != THEN) {
            ret = SysError("else unexpected");
        } else {
            ifStatus = ELSE;
            ret = 0;
        }
    } else if (!strcmp(keyword, "fi")) {
        if (ifStatus != ELSE) {
            ret = SysError("fi unexpected");
        } else {
            ifStatus = NEUTRAL;
            ret = 0;
        }
    } else {
        Fatal("internal processing error", 3);
    }
    return ret;
}
int Process(char *argList[]) {
    int ret = 0;
    if (!*argList) return 0;
    if (IsControlCommand(*argList)) {
        ret = DoControlCommand(argList);
    } else if (OkToExecute()) {
        if (!BuildInCommand(argList, &ret)) {
            ret = Execute(argList);
        }
    }
    return ret;
}
void FreeArgList(char *argList[]) {
    if (argList) {
        char **ptr = argList;
        while (*ptr) {
            free(*ptr++);
        }
        free(argList);
    }
}
void PrintResult(int result) {
    int ret, sig, core;
    ret = result >> 8;
    sig = result & 0x7F;
    core = result & 0x80;
    printf("[exit val:%d, core:%d, signal:%d]\n", ret, core, sig);
    if (sig) printf("signal:%s\n", strsignal(sig));
}
int main(int argc, char *argv[]) {
    //    setbuf(stdout, NULL);
    char *command;
    char **argList;
    int result;
    int len;
    VEnviron2Table(environ);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
     while (command = GetCommand(stdin, &len)) {
        if (argList = GetArgList(command, len)) {
            result = Process(argList);
        }
        FreeArgList(argList);
    }
     free(command);
    return 0;
}
