#include <stdio.h> //입출력
#include <stdlib.h> //문자열 변환, 의사 난수 생성
#include <unistd.h> //표준 심볼 상수 및 자료형
#include <string.h> //문자열 상수
#include <arpa/inet.h> //주소변환
#include <sys/socket.h> //소켓 연결
#include <netinet/in.h> //IPv4 전용 기능
#include <pthread.h> //쓰레드 사용

#define BUF_SIZE 100 //클라인터로부터 전송받을 문자열의 길이
#define MAX_CLNT 256 //최대 접속할수있는 클라이언트의 갯수(생성클라이언트 소캣 갯수)

void * handle_clnt(void * arg); //클라이언트로 부터 받은 메시지를 처리
void send_msg(char * msg, int len); //클라이언트로 부터 메시지를 받아옴
void error_handling(char * msg); //에러 처리

int clnt_cnt=0; //클라이언트 수 카운팅
//여러명의 클라이언트가 접속하므로 클라이언트 소켓은 배열입니다.
//멀티쓰레드 시 이 두 전역변수 clnt_cnt, clnt_socks 에 여러 쓰레드가 동시 접근할 수 있기에 두 변수의 사용이 들어간다면 무조건 임계영역입니다.
int clnt_socks[MAX_CLNT]; //최대 클라이언트 소켓 클라이언트 최대 256명
pthread_mutex_t mutx; //mutex : 쓰레드 동시접근 해결 (다중 스레드끼리 전역변수 사용시 데이터의 혼선 방지)

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; //서버소켓, 클라이언트 소켓 변수 선언
	struct sockaddr_in serv_adr, clnt_adr; //서버,클라이언트 주소 변수 선언
	int clnt_adr_sz; //클라이언트 주소 크기
	pthread_t t_id; //쓰레드 아이디
	if(argc!=2) { //prot번호 받기 + 파일명
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL); //mutex 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);  //서버 소켓 연결
	// 첫번째 인자: ip 버전 4프로토콜을 사용
	//두번째 인자: 스트림통신 즉 TCP를 이용(연결지향적)
	//세번째 인자: TCP/UDP를 정해주는 거인데 이미 첫번째 두번째 인자를 통해서 당연히
	//IPPROTO_TCP가 되어서 0으로 해도 됩니다.
	memset(&serv_adr, 0, sizeof(serv_adr)); //초기화
	serv_adr.sin_family=AF_INET; //addr family -> AF_INET : internet family IPv4
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //서버 IP 지정
	serv_adr.sin_port=htons(atoi(argv[1]));//서버 port
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) //주소할당
    //5로 지정을 했으니 5명까지만 가능한 채팅이 아니라 큐의 크기가 5이고 운영체제가 여유가 된다면 알아서 accept하고 총 256명까지
    //접속이 가능합니다.
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1) //listen
		error_handling("listen() error");
    //종료조건이 없으므로 ctrl + c 로만 종료합니다
	while(1) //값 받아오기
	{
		clnt_adr_sz=sizeof(clnt_adr); //크기 할당
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz); //소켓 할당
		//clnt_socks[], clnt_cnt 전역변수를 사용하기 위해 뮤텍스를 잠금니다.
		pthread_mutex_lock(&mutx); //쓰레드 잡기 , 클라이언트 카운터 올리고, 소켓배정
		clnt_socks[clnt_cnt++]=clnt_sock; //클라이언트 소켓 받기
		pthread_mutex_unlock(&mutx); // 쓰레드 놓아줌 
	    // 쓰레드 생성 쓰레드의 main 은 handle_clnt
        // 네 번째 파라미터로 accept 이후 생겨난 소켓의 파일 디스크립터 주소값을 넣어주어
        // handle_clnt 에서 파라미터로 받을 수 있도록 합니다.
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); //쓰레드 생성, handle_clnt함수 실행
        //만약에 스레드가 null값을 리턴했다면 스레드종료
		pthread_detach(t_id); //쓰레드 소멸
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
    //ctrl + c 로 프로그램 조욜시 서버 소켓 종료
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg) //클라이언트가 보낸 메시지를 핸들링
{
    //서버 파일 디스크럽트가 void 포인터로 돌아왔으므로 int로 형변환
	int clnt_sock=*((int*)arg); //클라이언트 소켓 할당
	int str_len=0, i; //문자열 길이 선언
	char msg[BUF_SIZE]; //메시지 크기 설정(100)
	
    //클라이언트에서 보낸 메세지 받고 클라이언트에서 EOF 보내서
    //str_len이 0이 될때까지 반복합니다.
    //클라이언트가 접속을 하고 있는 동안에 while 문을 벗어나진 않습니다.
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) //클라이언트가 보낸 메시지를 읽음
		send_msg(msg, str_len); // 메시지 읽는 함수실행
	
    //while문을 나왔다는건 현재 담당하는 소켓의 연결이 끊어졌다는 것 입니다.
    //clnt_socks[]에서 삭제하고 스레드도 소멸시켜야합니다.

	pthread_mutex_lock(&mutx); //먼저 생선된 쓰레드를 보호 + 잠금니다
	for(i=0; i<clnt_cnt; i++)   // 연결 끊어진 클라이언트인 현재 스레드에서 담당하는 소켓을 삭제합니다.
	{
        //현재 담당하는 클라이언트 소켓의 파일 디스크럽터 위치를 찾으면
        //현재 소켓이 원래 위치했던 곳을 기준으로 뒤의 클라이언트들을 가지고 옵니다.
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
    //클라이언트 수를 하나 줄입니다.
	clnt_cnt--;
	pthread_mutex_unlock(&mutx); //쓰레드를 보호에서 해제하고 클라이언트 소켓을 종료합니다.
	close(clnt_sock);
	return NULL;
}

void send_msg(char * msg, int len) //mutex로 잡고있는 모든 내용을 보냄니다.
{
    //clnt_cnt, clnt_socks[] 사용위해 뮤텍스를 잠금니다.
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++) 
    //모두에게 메세지를 보냅니다.
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx); // 뮤텍스 잠금해제
}

void error_handling(char * msg) //에러 핸들링 코드
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

