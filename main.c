#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const unsigned int width=16;
const unsigned int height=16;
const _Bool wrap=1;
unsigned int rulesS[9]={0,0,1,1,0,0,0,0,0};
unsigned int rulesR[9]={0,0,0,1,0,0,0,0,0};
unsigned int curBuffer=0;
int steps=-1;
int interval=-1;

struct Command{
    const unsigned int length;
    const char command[8];
};

struct Command commands[7]={
    {
        .length=3,
        .command={'e','x','i','t'}
    },
    {
        .length=3,
        .command={'s','t','e','p'}
    },
    {
        .length=4,
        .command={'s','t','a','t','e'}
    },
    {
        .length=2,
        .command={'s','e','t'}
    },
    {
        .length=2,
        .command={'g','e','t'}
    },
    {
        .length=2,
        .command={'r','u','n'}
    },
    {
        .length=4,
        .command={'r','u','l','e','s'}
    }
};

void draw(const unsigned int buffer[]){
    unsigned int curLine=0;
    char lines[height][width+1];
    for(unsigned int i=0;i<height;i++) {
        lines[i][width]='\0';
    }
    for(int i=0;i<width*height/16;i++){
        for(int p=0;p<16;p++){
            if((buffer[i]&(1<<p))>>p==1){
                lines[curLine][(p+i*16)%width]='#';
            }else{
                lines[curLine][(p+i*16)%width]=' ';
            }
            if((p+i*16)%width==width-1){
                curLine++;
            }
            if(curLine>height){
                break;
            }
        }
        if(curLine>height){
            break;
        }
    }
    system("cls");
    for(int i=0;i<height;i++) {
        printf("%s\n\0",lines[i]);
    }
}
unsigned int readBuffer(const unsigned int buffer[],const int x,const int y){
    int wx;
    int wy;
    if(x>=width||x<0){
        if(wrap==0){return(0);}
        wx=(x%(int)width+(int)width)%(int)width;
    }else{
        wx=x;
    }
    if(y>=height||y<0){
        if(wrap==0){return(0);}
        wy=(y%(int)height+(int)height)%(int)height;
    }else{
        wy=y;
    }
    return((buffer[((wx)+(wy)*width)/16]&(1<<((wx)+(wy)*width)%16))>>((wx)+(wy)*width)%16);
}
void writeBuffer(unsigned int buffer[],const int x,const int y,const _Bool state){
    if(state){
        buffer[((x)+(y)*width)/16]=buffer[((x)+(y)*width)/16]|(1<<(((x)+(y)*width)%16));
    }else{
        buffer[((x)+(y)*width)/16]=buffer[((x)+(y)*width)/16]&(0xFFFF-(1<<((x)+(y)*width)%16));
    }
}

unsigned int livingAdj(const unsigned int buffer[],int x, int y){
    return(
        readBuffer(buffer,x+1,y-1)+
        readBuffer(buffer,x+1,y+0)+
        readBuffer(buffer,x+1,y+1)+
        readBuffer(buffer,x+0,y-1)+
        readBuffer(buffer,x+0,y+1)+
        readBuffer(buffer,x-1,y-1)+
        readBuffer(buffer,x-1,y+0)+
        readBuffer(buffer,x-1,y+1)
    );
}

void update(unsigned int bufferA[],unsigned int bufferB[]){
    if(curBuffer==0){
        for(int x=0;x<width;x++){
            for(int y=0;y<height;y++){
                unsigned int adj=livingAdj(bufferA,x,y);
                if(rulesS[adj]==0){writeBuffer(bufferB,x,y,0);continue;}
                writeBuffer(bufferB,x,y,readBuffer(bufferA,x,y)||rulesR[adj]);
            }
        }
    }else{
        for(int x=0;x<width;x++){
            for(int y=0;y<height;y++){
                unsigned int adj=livingAdj(bufferB,x,y);
                if(rulesS[adj]==0){writeBuffer(bufferA,x,y,0);continue;}
                writeBuffer(bufferA,x,y,readBuffer(bufferB,x,y)||rulesR[adj]);
            }
        }
    }
    curBuffer=(curBuffer+1)%2;
}

int handleCmd(unsigned int buffer[],char cmd[],char arg0[],char arg1[],char arg2[],char arg3[]){
    int poss[7]={1,1,1,1,1,1,1};
    int cmdID=-1;
    for(int pos=0;pos<8;pos++){
        for(int i=0;i<7;i++){
            if(poss[i]==0){continue;}
            if(cmd[pos]!=commands[i].command[pos]){
                poss[i]=0;
                continue;
            }
            if(pos>=commands[i].length){cmdID=i;break;}
        }
        if(cmdID!=-1){break;}
    }
    if(cmdID==-1){return(0);}
    int x;
    int y;
    switch(cmdID){
        case(0):
            return(-1);
        case(1):
            steps=atoi(arg0);
            interval=-1;
            return(1);
        case(2):
            if(atoi(arg2)==-1){
                x=atoi(arg0);
                y=atoi(arg1);
                printf("%i,%i->%i",x,y,readBuffer(buffer,x,y));
                return(0);
            }else{
                x=atoi(arg0);
                y=atoi(arg1);
                int state=atoi(arg2);
                writeBuffer(buffer,x,y,state);
                return(1);
            }
        case(3):
            x=atoi(arg0);
            y=atoi(arg1);
            int state=atoi(arg2);
            writeBuffer(buffer,x,y,state);
            return(1);
        case(4):
            x=atoi(arg0);
            y=atoi(arg1);
            printf("%i,%i->%i",x,y,readBuffer(buffer,x,y));
            return(0);
        case(5):
            steps=atoi(arg0);
            interval=atoi(arg1);
            return(1);
        case(6):
            int rS=atoi(arg0);
            int rR=atoi(arg1);
            for(int i=0;i<9;i++){
                rulesS[i]=0;
                rulesR[i]=0;
            }
            while(1){
                int dig=rS%10;
                rulesS[dig]=1;
                rS=rS/10;
                if(rS==0){break;}
            }
            while(1){
                int dig=rR%10;
                rulesR[dig]=1;
                rR=rR/10;
                if(rR==0){break;}
            }
            return(0);
        default:
            return(0);
    }
}


int main(){

    unsigned int bufferA[width*height/16];
    unsigned int bufferB[width*height/16];

    srand(time(NULL));

    for(int i=0;i<width*height/16;i++){
        bufferA[i]=rand()%65536;
    }

    draw(bufferA);
    int handleRet;
    while(1){
        if(curBuffer==0){
            draw(bufferA);
        }else{
            draw(bufferB);
        }
        while(1){
            char cmd[8];
            char arg0[16];
            char arg1[16];
            char arg2[16];
            char arg3[16];
            scanf("%s %s %s %s %s",&cmd,&arg0,&arg1,&arg2,&arg3);
            if(curBuffer==0){
                handleRet=handleCmd(bufferA,cmd,arg0,arg1,arg2,arg3);
            }else{
                handleRet=handleCmd(bufferB,cmd,arg0,arg1,arg2,arg3);
            }
            if(handleRet==-1){return(0);}
            if(handleRet==1){break;}
            printf("\n");
        }
        while(steps>0){
            update(bufferA,bufferB);
            if(interval!=-1){
                usleep(interval*1000);
                if(curBuffer==0){
                    draw(bufferA);
                }else{
                    draw(bufferB);
                }
            }
            steps--;
        }
    }
}

/*
char* splitString(char* str,char splitter,int id) {

}
*/