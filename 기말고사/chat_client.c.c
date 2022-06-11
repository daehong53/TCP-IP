#include <stdio.h> //입출력
#include <stdlib.h> //문자열 변환, 의사 난수 생성
#include <unistd.h> //표준 심볼 상수 및 자료형
#include <string.h> //문자열 상수
#include <arpa/inet.h> //주소변환
#include <sys/socket.h> //소켓 연결
#include <pthread.h> //쓰레드 사용
	
#define BUF_SIZE 100 // 버퍼 사이즈
#define NAME_SIZE 20 //사용자 이름
	
void * send_msg(void * arg); //메시지를 보냄
void * recv_msg(void * arg); // 메시지를 받음
void error_handling(char * msg); // 에러 핸들링
	
char name[NAME_SIZE]="[DEFAULT]"; //이름 선언 및 초기화 + 본인 닉네임 20자 제한
char msg[BUF_SIZE]; // 메시지 선언
	
int main(int argc, char *argv[])
{
	int sock; // 소켓 선언
	struct sockaddr_in serv_addr; //소켓 주소 선언
    //송신 스레드와 수신 스레드로 2개의 스레드를 선언하고 내 메세지를 보내야하고 상대방의 메세지도
    //받아야합니다
	pthread_t snd_thread, rcv_thread; //보내고 받는 쓰레드 선언
    //pthread_join에서 사용합니다.
	void * thread_return; //ip, port, 이름 받기 + 사용자 이름까지 넣어줘야 합니다.
	if(argc!=4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	 }
	
    //argv[3] 이 hong 이라면 "[hong]"이 name 이 됩니다.
	sprintf(name, "[%s]", argv[3]);// 이름 입력 받음
	sock=socket(PF_INET, SOCK_STREAM, 0); //서버 소켓 연결
	
	memset(&serv_addr, 0, sizeof(serv_addr)); //초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) //연결
		error_handling("connect() error");
	
    //두 개의 스레드 생성하고 각각의 main은 send_msg, recv_msg
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock); //메세지 보냄
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock); // 메세지 받음

    //스레드 종료 대기 및 소멸 유도
	pthread_join(snd_thread, &thread_return); // send 쓰레드 소멸
	pthread_join(rcv_thread, &thread_return); // recv 쓰레드 소멸
	//클라이언트 연결 종료
    close(sock);   
	return 0;
}
	
void * send_msg(void * arg)   // sned스레드 메인
{
    //void형을 int형으로 전환합니다.
	int sock=*((int*)arg);
    //사용자 아이다와 메세지를 붙여서 한번에 보낼 것 입니다.
	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1) 
	{
        //입력받음
		fgets(msg, BUF_SIZE, stdin);
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
		{
            //q입력시 종료
            //서버에 EOF를 전송합니다.
			close(sock);
			exit(0);
		}
        //id를 hong, 메세지를 안녕이라고 했다면 [hong] 안녕이라고나오고
        //저 문자가 name_msg로 들어가서 출력하게 됩니다.
		sprintf(name_msg,"%s %s", name, msg);
        //서버로 메세지를 보냅니다.
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // rcv_thread의 스레드 메인
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
	while(1)
	{
        //서버에서 들어온 메세지를 수신합니다
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
        //str_len이 -1이라는 건 서버 소켓과 연결이 끊어졌다는 소리이고
        //send_msg 에서 close(sock)이 실행되고 서버로 EOF가 갔으면
        //서버는 그거를 받아서 자기가 가진 클라이언트 소켓을 close하고 그러면 읽었을때 결과가 -1일 것 입니다.
		if(str_len==-1) 
        //종료를 위한 리턴값이고 thread_return으로 갈 것입니다.
			return (void*)-1; //pthread_joing을 실행시키기 위해 추가
            
            //버퍼 마지막 값 NULL
		name_msg[str_len]=0;
        //받은 메세지 출력
		fputs(name_msg, stdout);
	}
	return NULL;
}
	
void error_handling(char *msg) //에러
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}