/* Author: bo8 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

typedef enum mau_quan_co {ddo, dden} mau_quan_co;
typedef enum loai_quan_co {xe, phao, ma, tuong, si, tuongj, chot} loai_quan_co;
char s[3][256] = {"Đây là cờ tướng, không phải cờ khích tướng!", "Tâm bất biến giữa cuộc đời vạn biến.", "Đừng nói những lời như vậy!"};

void read_str(char *buf, unsigned int size);
int read_num();
int read_quan();
char * read_dan_do();
void menu();
void hello();
void read_file(const char *file_path);
void get_flag();

typedef struct quan_co {
    char khac_cot_ghi_tam[32];
    loai_quan_co loai;
    mau_quan_co mau;
    char string[4];
    void (*di) (struct quan_co *);
} quan_co;

/* di function for quan_co */
void di_xe(quan_co *co);
void di_phao(quan_co *co);
void di_tuong(quan_co *co);
void di_default(quan_co *co);
/* init function for quan_co */
void init_quan_co(quan_co *co, loai_quan_co loai, char *dan_do, mau_quan_co mau);

struct key_value {
    char *key;
    loai_quan_co value;
    char *s_do;
    char *s_den;
    void (* func) (quan_co *);
} mapping[7] = {
    {"xe", xe, "俥", "車", di_xe},
    {"phao", phao, "炮", "砲", di_phao},
    {"ma", ma, "傌", "馬", di_default},
    {"tuong", tuong, "帥", "將", di_default},
    {"si", si, "仕", "士", di_default},
    {"tuongj", tuongj, "相", "象", di_tuong},
    {"chot", chot, "兵", "卒", di_default}
};

typedef struct player {
    int nuoc;
    int quan_luc;
    quan_co c[16];
    void (*di)(struct player *);
    void (*trash_talk)(struct player *);
    void (*them_quan)(struct player *, loai_quan_co, char *, mau_quan_co);
} player;

/* di function for human & ai */
void human(player *p);
void ai(player *p);
/* trash_talk function for human & ai */
void khich_tuong(player *p);
void ai_kich_tuong(player *p);
/* them_quan function for human */
void them_quan(player* p, loai_quan_co l, char *dan_do, mau_quan_co mau);
/* init function for human & ai */
void init_human(player **p);
void init_ai(player **p);

void main() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    srand(time(NULL));

    hello();

    player *ai;
    init_ai(&ai);

    player *human;
    init_human(&human);

    int quan;
    char *dan_do;

    while (1) {
        menu();
        switch (read_num()) {
            case 1:
                quan = read_quan();
                dan_do = read_dan_do();
                human->them_quan(human, quan, dan_do, ddo);
                printf("- Đã điều động thêm %s.\n", human->c[human->quan_luc-1].string);
                break;
            case 2:
                human->di(human);
                ai->di(ai);
                break;
            case 3:
                human->trash_talk(human);
                ai->trash_talk(ai);
                break;
            default:
                printf("Nghỉ chơi.\n");
                return ;
                break;
        }
    }
}

void print_quan_co(quan_co *co) {
    printf("%s", co->string);
}

void di_xe(quan_co *co) {
    printf("%s 1 tiến 2\n", co->string);
}

void di_phao(quan_co *co) {
    printf("%s 8 thoái 1\n", co->string);
}

void di_tuong(quan_co *co) {
    printf("%s 7 tấn 5\n", co->string);
}

void di_default(quan_co *co) {
    printf("%s đi ???\n", co->string);
}

void init_quan_co(quan_co *co, loai_quan_co loai, char *dan_do, mau_quan_co mau) {
    int i;

    strncpy(co->khac_cot_ghi_tam, dan_do, 32);
    co->loai = loai;
    co->mau = mau;

    for (i=0; i<7; i++) {
        if (loai == mapping[i].value) {
            if (mau == ddo) 
                strcpy(co->string, mapping[i].s_do);
            else if (mau == dden)
                strcpy(co->string, mapping[i].s_den);
            else
                strcpy(co->string, "???");

            co->di = mapping[i].func;
            break;
        }
    }

    if (i==7) {
        strcpy(co->string, "???");
        co->di = di_default;
    }

}

void read_str(char *buf, unsigned int size) {
    int ret;
    ret = read(0, buf, size);

    if (ret <= 0) {
        puts("read error");
        exit(1);
    }

    buf[ret-1] = '\x00';
}

int read_num() {
    char buf[64];
    read_str(buf, 64);
    return atoi(buf);
}

int read_quan() {
    char buf[10];
    printf("Điều động thêm ai (xe/phao/ma/tuong/si/tuongj/chot)? ");
    read_str(buf, 10);
    int i;
    for (i=0; i<7; i++) {
        if (strcmp(buf, mapping[i].key) == 0)
            return mapping[i].value;
    }
    return -1;
}

char * read_dan_do() {
    char buf[32];
    printf("Hãy dặn dò trước xuất trận: ");
    read_str(buf, 32);
    return strdup(buf);
}

void menu() {
    printf("1. Điều động thêm quân lực\n");
    printf("2. Đi\n");
    printf("3. Khích tướng\n");
    printf("4. Buông cờ\n");
    printf("> ");
}

void hello() {
    printf("----------- Human vs AI -----------\n");
    printf("\t車馬象士將士象馬車\n");
    printf("\t　　　｜Ｘ｜　　　\n");
    printf("\t　砲　＋－＋　砲　\n");
    printf("\t卒　卒　卒　卒　卒\n");
    printf("\t－－－－－－－－－\n");
    printf("\t－－－－－－－－－\n");
    printf("\t兵　兵　兵　兵　兵\n");
    printf("\t　炮　＋－＋　炮　\n");
    printf("\t　　　｜Ｘ｜　　　\n");
    printf("\t俥傌相仕帥仕相傌俥\n");
    printf("-----------------------------------\n");
}

void human(player *p) {
    p->nuoc += 1;
    printf("- Human: Tôi chưa suy nghĩ xong, mời đi!\n");
}

void ai(player *p) {
    int r;
    p->nuoc += 1;
    printf("- AI: ");
    switch (p->nuoc) {
        case 1:
            p->c[10].di(&(p->c[10]));
            break;
        case 2:
            p->c[0].di(&(p->c[0]));
            break;
        case 3:
            p->c[2].di(&(p->c[2]));
            break;
        default:
            r = rand() % 16;
            p->c[r].di(&(p->c[r]));
            break;
    }
}

void khich_tuong(player *p) {
    char buf[128];
    printf("- Human: ");
    read_str(buf, 128);
}

void ai_kich_tuong(player *p) {
    printf("- AI: %s\n", s[rand() % 3]);
}

void them_quan(player* p, loai_quan_co l, char *dan_do, mau_quan_co mau) {
    init_quan_co(&(p->c[p->quan_luc]), l, dan_do, mau);
    p->quan_luc += 1;
}

void init_human(player **p) {
    *p = (player *)malloc(sizeof(player));
    (*p)->nuoc = 0;
    (*p)->quan_luc = 0;
    (*p)->di = human;
    (*p)->trash_talk = khich_tuong;
    (*p)->them_quan = them_quan;
}

void init_ai(player **p) {
    *p = (player *)malloc(sizeof(player));
    (*p)->nuoc = 0;
    (*p)->quan_luc = 0;
    (*p)->di = ai;
    (*p)->trash_talk = ai_kich_tuong;
    (*p)->them_quan = them_quan;

    loai_quan_co lqc[16] = {xe, xe, phao, phao, ma, ma, tuong, si, si, tuongj, tuongj, chot, chot, chot, chot, chot};
    int i;
    for (i=0; i<16; i++) {
        (*p)->them_quan((*p), lqc[i], "Cố lên!", dden);
    }
}

void read_file(const char *file_path) {
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;

    pFile = fopen ( file_path , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */
    printf("%s", buffer);

    // terminate
    fclose (pFile);
    free (buffer);
    return ;
}

void get_flag() {
    printf("Life is too hard, so I make it easy for u ^^\n");
    read_file("/home/cokhichtuong/flag");
}