#include <arpa/inet.h>
#include <capstone/capstone.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void errquit(char *errmsg)
{
    fprintf(stderr, "%s\n", errmsg);
    exit(-1);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        errquit("Fail to create a socket");

    // host information
    struct hostent *hostname = gethostbyname("aup.zoolab.org");
    char *host_address = inet_ntoa(*((struct in_addr *) hostname->h_addr));

    // socket
    struct sockaddr_in server;
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr(host_address);
    server.sin_port = htons(2530);
    if ((connect(sockfd, (struct sockaddr *) &server, sizeof(server))) < 0)
        errquit("Connect error");

    // initialize capstone engine
    static csh cshandle = 0;
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &cshandle) != CS_ERR_OK)
        errquit("Initializing capstone error\n");

    for (int loop = 0; loop < 10; ++loop) {
        // receiving messages
        char *recvMsg = calloc(0x400, sizeof(char));
        recv(sockfd, recvMsg, 0x400, 0);

        // extract the quiz assembly string
        char quiz[100];
        // find string that matches regex `^>>>` then get the rests
        // NOTE the '*' before the regex tells sscanf to ignore the match
        // contents
        sscanf(recvMsg, "%*[^>>>]>>> %s", quiz);
        // add "\x" before each hexdecimal value
        uint8_t *quiz_res = calloc(0x64, sizeof(uint8_t));
        int quiz_res_count = 0;
        char *tmp = calloc(0x2, sizeof(char));
        for (int i = 0; i < strlen(quiz); i += 2) {
            strncpy(tmp, quiz + i, 2);
            quiz_res[quiz_res_count++] = strtol(tmp, NULL, 16);
        }
        free(tmp);
        free(recvMsg);

        // disassemble
        cs_insn *insn;
        size_t count;
        if ((count = cs_disasm(cshandle, quiz_res, quiz_res_count, 0, 0,
                               &insn)) <= 0)
            errquit("Disassemble error");
        free(quiz_res);
        char *instruction = calloc(0x100, sizeof(char));
        for (int j = 0; j < count; j++) {
            strncat(instruction, insn[j].mnemonic, strlen(insn[j].mnemonic));
            strncat(instruction, " ", 1);
            // `test` instruction will get inverted output from Capstone library
            // e.g. "test rax, rbx" will get "test rbx, rax" here which is
            // opposite of the answer, so I patch manually here. (python
            // Capstone works same as the answer)
            if (!strncmp(insn[j].mnemonic, "test", 4)) {
                char *tmp = calloc(0x3, sizeof(char));
                strncpy(tmp, insn[j].op_str + 5, 3);
                strncat(instruction, tmp, 3);
                strncat(instruction, ", ", 2);
                strncpy(tmp, insn[j].op_str, 3);
                strncat(instruction, tmp, 3);
                free(tmp);
            } else
                strncat(instruction, insn[j].op_str, strlen(insn[j].op_str));
            strncat(instruction, "\n", 1);
        }

        // convert assembly string to ascii order and send
        int ord = 0;
        char ord_chr[3];
        ord_chr[2] = '\0';
        char *instruction_ord = calloc(0x400, sizeof(char));
        for (int i = 0; i < strlen(instruction); ++i) {
            if ((int) instruction[i] == 10)
                sprintf(ord_chr, "0a");
            else
                sprintf(ord_chr, "%x", (int) instruction[i]);
            strncat(instruction_ord, ord_chr, sizeof(ord_chr));
        }
        instruction_ord[strlen(instruction_ord)] = '\n';
        send(sockfd, instruction_ord, strlen(instruction_ord), 0);

        free(instruction);
        free(instruction_ord);
    }
    char *success = calloc(0x100, sizeof(char));
    recv(sockfd, success, 0x100, 0);
    printf("%s\n", success);
    free(success);

    // close capstone engine
    cs_close(&cshandle);

    return 0;
}
