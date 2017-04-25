#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#define PORT "3758" //the port user will be connecting to
#define PENDING 3  //how many pending connection queue will hold
//#define mx_users 4
#define LOGIN_OK "OK"
#define level1_threshold 30
#define level2_threshold 90
//#define level3_threshold 40
//#define LOGIN_WrongPassword "NP"
//#define LOGIN_WrongUsername "NU"
#define MX_USERS 4
#define WAIT_ATIME 15
#define WAIT_LTIME 12
#define mx_ques 10


typedef struct QnA
{
	char ques[5000];
	char ans; 
	int valid;
}QnA;

int flagc = 1;
void handler_connectc()
{
	flagc = 0;
}
QnA hp[3][51];
QnA cs[3][51];
QnA ca[3][51];

struct QuesFiles{
	char Qfile[1024];
};
struct AnsFiles
{
	char Afile[1024];
};
struct QuesFiles fpQues[3][3]={
	{ {.Qfile="hq1.txt"},{.Qfile="hq2.txt"},{.Qfile="hq3.txt"}},
	{{.Qfile="cpq1n.txt"},{.Qfile="cpq2n.txt"},{.Qfile="cpq3n.txt"}},
	{{.Qfile="gkq1n.txt"},{.Qfile="gkq2n.txt"},{.Qfile="gkq3n.txt"}}
};

struct AnsFiles fpAns[3][3]={
	{ {.Afile="h1.txt"},{.Afile="h2.txt"},{.Afile="h3.txt"}},
	{{.Afile="cp1n.txt"},{.Afile="cp2n.txt"},{.Afile="cp3n.txt"}},
	{{.Afile="gk1n.txt"},{.Afile="gk2n.txt"},{.Afile="gk3n.txt"}}
};
int threshold_score[2] = {level1_threshold, level2_threshold};

int flag=1;
int firstcon=1;

int category=0;
void handler_sa(int s)
{
	flag=0;
	//printf("in handler\n");
}
void handler_firstcon(int s)
{
	firstcon=0;
	//printf("in first handler\n");
}

struct user
{
	int socket;
	char userName[20];
	int level;
	int score;
	int category;
	int visibility;
};
typedef struct user user;
user user_list[MX_USERS];
int user_id=0;
int mode = 0;

void InitialiseQues (FILE* fp,int level,int category)
{
	int i=1,j;
	char ch;
	switch(category)
	{
		case 1 : //printf("Ques case 1 \n"); 
				while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != '$' && fp != NULL && !feof(fp))
                    {
                    	//printf("while inner \n");
                    	hp[level-1][i].ques[j]=ch;
                    	ch= fgetc(fp);
                    	j++;	
                    }
                    hp[level-1][i].ques[j]='\0';
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
        case 2:while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != '$'&& fp != NULL && !feof(fp))
                    {
                    	cs[level-1][i].ques[j]=ch;
                    	ch= fgetc(fp);
                    	j++;	
                    }
                    cs[level-1][i].ques[j]='\0';
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
        case 3:while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != '$'&& fp != NULL && !feof(fp))
                    {
                    	ca[level-1][i].ques[j]=ch;
                    	ch= fgetc(fp);
                    	j++;	
                    }
                    ca[level-1][i].ques[j]='\0';
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
	}

}

void InitialiseAns (FILE* fp,int level,int category)
{
	int i=1,j;
	char ch, ch1 ='X';
	switch(category)
	{
		case 1 : while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != ' ' &&ch != '\n'&& fp != NULL && !feof(fp))
                    {
                    	//if(ch != )
                    	//printf("ans for %c\n", ch);
                    	hp[level-1][i].ans=ch;
                    	
                    	ch= fgetc(fp);
                    	j++;	
                    }
                    
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
        case 2:while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != '\n'&& fp != NULL && !feof(fp))
                    {
                    	//printf("ans for %c\n", ch);
                    	cs[level-1][i].ans=ch;
                    	//printf("ans in array for %c\n", hp[level-1][i].ans);
                    	ch= fgetc(fp);
                    	j++;	
                    }
                 
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
        case 3:while(fp != NULL && !feof(fp))
                {
                	j=0;
                    ch = fgetc(fp);
                    while(ch != '\n'&& fp != NULL && !feof(fp))
                    {
                    	//printf("ans for %c\n", ch);
                    	ca[level-1][i].ans=ch;
                    	//printf("ans in array for %c\n", hp[level-1][i].ans);
                    	ch= fgetc(fp);
                    	j++;	
                    }
                    
                    i++;
                    // if (!feof(fpClean))
                    //     printf("%c",ch);
                }
                break;
	}

}


int register_fn(int newSocket)
{
	char recv_buf [1024], sending_buffer[1024];
	// Enter username
	strcpy(sending_buffer, "Enter Username : \n");
 	send  ( newSocket, sending_buffer, 1024, 0 );
	if(recv(newSocket,recv_buf,1024,0)== -1)	
	{
		printf("[ERROR] Client failed to send username.\n");
		// ----------------------------------continue-------------------------------------------------------------------------------n
	}
	printf("Entered username- %s\n",recv_buf);
	user_list[user_id].socket = newSocket;
	strcpy(user_list[user_id].userName, recv_buf);
	// Enter mode
	//printf("%d mode entered\n",mode );
	if(mode == 0)// game not yet started
	{
		//printf("inside send mode 0 \n");
		strcpy(sending_buffer, "Available mode [1 : SINGLEPLAYER / 2 : MULTIPLAYER] \nEnter mode : \n");
		if(send  ( newSocket, sending_buffer, 1024, 0 )== -1 )
		{
			printf("Could not send mode info \n");
		}
		//printf("%s\n",sending_buffer);
	}
	else if(mode == 1) // more than 1 single players
	{
		strcpy(sending_buffer, "Available mode [1 : SINGLEPLAYER] : \nEnter mode : \n");
		send  ( newSocket, sending_buffer, 1024, 0 );
	}
	else if(mode == 2) // all mutli players
	{
		strcpy(sending_buffer, "Available mode [2 : MULTIPLAYER] : \nEnter mode : \n");
		send  ( newSocket, sending_buffer, 1024, 0 );
	}
	
	// strcpy(sending_buffer, "Enter mode : \n");
	// send  ( newSocket, sending_buffer, 1024, 0 );
	if(recv(newSocket,recv_buf,1024,0)== -1)	
	{
		printf("[ERROR] Client failed to send mode.\n");
	}
	mode = atoi(recv_buf);
	printf("Entered mode %d\n",mode);
	// Enter category
	switch(mode)
	{
		case 1 :strcpy(sending_buffer, "Available category [1: HARRY POTTER / 2 : BASICS OF COMPUTER SCIENCE/ 3: GENERAL KNOWLEDGE] \nEnter category : \n");
				send  ( newSocket, sending_buffer, 1024, 0 );
			 	break;
		
		case 2 : if(category == 0)// game not yet started
				{
					strcpy(sending_buffer, "Available category [1: HARRY POTTER / 2 : BASICS OF COMPUTER SCIENCE/ 3: GENERAL KNOWLEDGE] \nEnter category : \n");
					send  ( newSocket, sending_buffer, 1024, 0 );
				}
				else if(category == 1) //player 1 chose category as h
				{
					strcpy(sending_buffer, "Available category [1: HARRY POTTER ] : \nEnter category : \n");
					send  ( newSocket, sending_buffer, 1024, 0 );
				}
				else if(category == 2) // player 1 chose category as b
				{
					strcpy(sending_buffer, "Available category [ 2 : BASICS OF COMPUTER SCIENCE] : \nEnter category : \n");
					send  ( newSocket, sending_buffer, 1024, 0 );
				}
				else if(category == 3) // player 1 chose category as c
				{
					strcpy(sending_buffer, "Available category [ 3: GENERAL KNOWLEDGE] : \nEnter category : \n");
					send  ( newSocket, sending_buffer, 1024, 0 );
				}
	
			 break;
		default : strcpy(sending_buffer, "Wrong mode. connection closing \n");
					send  ( newSocket, sending_buffer, 1024, 0 );
					return 0;
	}

	
	// strcpy(sending_buffer, "Enter category : \n");
	// send  ( newSocket, sending_buffer, 1024, 0 );
	if(recv(newSocket,recv_buf,1024,0)<=0)	
	{
		printf("[ERROR] Client failed to send category.\n");
	}
	user_list[user_id].category=atoi(recv_buf);
	printf("Entered category %d\n",user_list[user_id].category);
	category = atoi(recv_buf);
	//printf("Entered category %d\n",category);
	// Enter level
	switch(mode)
	{
		case 1 : strcpy(sending_buffer, "Open level : 1 2 3\nEnter level you want to play : \n");
			 send  ( newSocket, sending_buffer, 1024, 0 );
			 break;
		
		case 2 : strcpy(sending_buffer, "Open level : 1\nEnter level you want to play : \n");
			 send  ( newSocket, sending_buffer, 1024, 0 );
			 break;
	}
	// strcpy(sending_buffer, "Enter level you want to play : \n");
	// send  ( newSocket, sending_buffer, 1024, 0 );
	 if(recv(newSocket,recv_buf,1024,0)<=0)	
	{
		printf("[ERROR] Client failed to send level.\n");
	}
	user_list[user_id].level = atoi(recv_buf);
	printf("Entered level %d\n",user_list[user_id].level);
	//Enter visibility

	switch(mode)
	{
		
		case 1 : strcpy(sending_buffer, "Available Visibility Setting [ 0 : PRIVATE MODE ] \nEnter Visibility : \n");
 				 send  ( newSocket, sending_buffer, 1024, 0 );
				 break;

		case 2 : strcpy(sending_buffer, "Available Visibility Settings [ 0 : PRIVATE MODE / 1 : PUBLIC MODE ] \nEnter Visibility : \n");
 				 send  ( newSocket, sending_buffer, 1024, 0 );
				 break;
		
	}
	
	if(recv(newSocket,recv_buf,1024,0)<= 0)	
	{
		printf("[ERROR] Client failed to send VISIBILITY.\n");
		if(user_id < 1)
		{
			printf("new error\n");
			exit(1);

		}
	}
	user_list[user_id].visibility = atoi(recv_buf);
	printf("Entered visibility %d\n",user_list[user_id].visibility);
	// Enter score
	user_list[user_id].score = 0;
	return mode;
}

int sendQues(fd_set temp,int maxfd,int i_Q)
{
		srand(time(NULL));
		int Qno=rand()%51;
		int i;
		
		//printf("inside sendques\n");
		for(i=0;i<MX_USERS;i++)
		{
			if(FD_ISSET(user_list[i].socket,&temp))
			{
				char ques_buf[5000];
			sprintf(ques_buf,"Q%d.",i_Q);
				//printf("category %d for i = %d level %d Qno - %d-------------\n",user_list[i].category,i,user_list[i].level, Qno);
			 	switch(user_list[i].category)
				{
					case 1: while(Qno == 0 || hp[user_list[i].level-1][Qno].valid == 0)
							{
								// if(Qno == 0)
								// 	//printf("i m Q 0\n");
								Qno=rand()%51;//51
							}
							//hp[user_list[i].level-1][Qno].valid = 0;
							printf("category %d level %d Qno - %d.\n",category,user_list[i].level, Qno);
							strcat(ques_buf,hp[user_list[i].level-1][Qno].ques);
							break;

					case 2: while(Qno == 0 || cs[user_list[i].level-1][Qno].valid == 0)
							{
								// if(Qno == 0)
								// 	printf("i m Q 0\n");
								Qno=rand()%51;
							}
							//cs[user_list[i].level-1][Qno].valid = 0;
							printf("category %d level %d Qno - %d.\n",category, user_list[i].level, Qno);
							
							strcat(ques_buf,cs[user_list[i].level-1][Qno].ques);
							break;

					case 3: while(Qno == 0 || ca[user_list[i].level-1][Qno].valid == 0)
							{
								// if(Qno == 0)
								// 	printf("i m Q 0\n");
								Qno=rand()%51;
							}
							//ca[user_list[i].level-1][Qno].valid = 0;
							printf("category %d level %d Qno - %d.\n",category, user_list[i].level-1, Qno);
							
							strcat(ques_buf,ca[user_list[i].level-1][Qno].ques);
							break;


				}
				
				
			    if(send( user_list[i].socket, ques_buf, 5000, 0 )!=5000)
				{
					printf("Error while sending question to %d ",i);
				}
				switch(user_list[i].category)
				{
					case 1: 
							hp[user_list[i].level-1][Qno].valid = 0;
							//printf("%d not valid\n",Qno );
							break;

					case 2: 
							cs[user_list[i].level-1][Qno].valid = 0;
							//printf("%d not valid\n",Qno );
							break;

					case 3: 
							ca[user_list[i].level-1][Qno].valid = 0;
							//printf("%d not valid\n",Qno );
							break;

				}
			}

		}
		
		//printf("out sendques\n");
	return Qno;

}


int checkans (char user_ans,int i ,int Qno)
{
	int category = user_list[i].category;
	//printf("category %d\n", user_list[i].category);
	//printf("in checkans\n");
	char correct_ans;
	switch(category)
	{
		case 1: //printf("checking ans\n");
				//printf("Qno %d, answer in check ans %c\n",Qno, hp[user_list[i].level-1][Qno].ans);
				correct_ans = hp[user_list[i].level-1][Qno].ans;
				break;
		case 2: correct_ans = cs[user_list[i].level-1][Qno].ans;
				break;
		case 3: correct_ans = ca[user_list[i].level-1][Qno].ans;
				break;

	}
	printf("user ans = %c correct ans = %c\n", user_ans, correct_ans);

	if(toupper(user_ans) == correct_ans)//(strcmp(correct_ans, user_ans)== 0)// == decrypt(user_ans))
	{
		printf("score shd be = %d\n", 10*user_list[i].level);
		return 10*user_list[i].level;
	}
	return 0;
}

char decrypt_fn(char* ans)
{
	int e_val = atoi(ans);
	char c;
	c = (e_val+11)/2;
	return c;
}

void recvAns(fd_set* clients,int maxfd,int Qno)
{
	
		flag=1;
		int i;		
		fd_set recvfds;
		fd_set temp;
		FD_ZERO(&recvfds);
		FD_ZERO(&temp);
		recvfds=*clients;
		char ans_buf[10];
		signal(SIGALRM,handler_sa);
		int user_cnt=user_id;
		//printf("no of usersssss %d\n", user_cnt);
		int cloop = 0;
		alarm(WAIT_ATIME);//while loop ends after alarm goes off as flag becomes 0
		while(flag && user_cnt!=0)//server waits to receive ans from clients for only wait_atime ...timer starts after sending question to all clients
		{	
		//	printf("loop %d\n", cloop++);
			if(user_cnt==0)
			{
				//printf("No user\n");
				//alarm(0);
				break;
			}
			temp=recvfds;//becoz select will modify temp and when ans is received from a client then it is removed from recvfds so tht we dont recv ans from it again
			if(select(maxfd+1, &temp, NULL, NULL, NULL) <=0)
			{
                perror("select");
               // printf("hello\n");
                //alarm(0);
                //exit(4);
                break;//break is important. Explained above
            }
           // printf("after select\n");
   //          for(i = 0; i <= maxfd; i++) 
			// {
			// 	printf("inside for\n");
   //              if(FD_ISSET(i, &temp)) 
			// 	{
			for(i=0;i<MX_USERS;i++)
			{
				if(FD_ISSET(user_list[i].socket,&temp))
				{
					//printf("indise if\n");
					if(recv(user_list[i].socket,ans_buf,10,0)!= sizeof(ans_buf))//check conditions for all 0/-1 etc
					{

						printf("error.Closing connection with %d\n",i);
						FD_CLR(i,&temp);
						FD_CLR(i,&recvfds);
						FD_CLR(i,clients);	
					}
					else
					{
						//printf("receiving ans\n");
						//char AnswerRec = ans_buf[0];
						//int user_ans = atoi(AnswerRec);//if user is unable to give any ans send a special char for it 
						char AnswerRec = decrypt_fn(ans_buf);
						printf("answer rec = %c\n", AnswerRec);
						user_list[i].score+=checkans(AnswerRec, i,Qno);
						FD_CLR(i,&temp);
						FD_CLR(i,&recvfds);	
						
					}
					user_cnt--;
				}
			}
		}
		while(flag)
		{
			//printf("lala\n");
		}
}

int foundWinner(fd_set* clients, int maxfd)
{
	//printf("inside found winner\n");
	int i = 0;
	int mx_score = 0, cnt = 0;
	int winners[MX_USERS];
	char win_buf[1024];
	for(i=0;i<MX_USERS;i++)
	{
		if(FD_ISSET(user_list[i].socket,clients))
		{
			if(user_list[i].level != 3)
			{
				return 0;
			}
			
			if(mx_score <= user_list[i].score)
			{
				mx_score = user_list[i].score;
			}
		}
	}
	
	for(i=0;i<MX_USERS;i++)
	{
		if(FD_ISSET(user_list[i].socket,clients))
		{
			if(mx_score == user_list[i].score)
			{
				winners[cnt++] = i;
			}
		}

	}		
	
	if(cnt == 1)
	{
		//printf("inside cnt 1\n");
		for(i=0;i<MX_USERS;i++)
		{
			if(FD_ISSET(user_list[i].socket,clients))
			{
				//printf("hello\n");
					sprintf(win_buf,"%s WON \n ********************** GAME OVER ***********************\n", user_list[winners[0]].userName);
					send(user_list[i].socket, win_buf,1024,0);// winner[0] was here		

			}
		}
		return 1;
	}
	
	for(i=0;i<MX_USERS;i++)
		{
			if(FD_ISSET(user_list[i].socket,clients))
			{
					strcpy(win_buf,"TIE AMONG ");
					int j=0;
					for(j=0;j<cnt;j++)
					{
						strcat(win_buf,user_list[winners[j]].userName);
					}
					strcat(win_buf,"\n *************************** GAME OVER *****************************\n");
					send(user_list[i].socket, win_buf,1024,0);	//winner[0]	
					
			}
		}
		return 1;


}

int updatePlayers(fd_set* clients,int maxfd)
{
	int i,gameover=1;
	char send_buf[1024]="";
	char recv_buf[10];
	int quit=1;
	int cnt = 0, lastActivePlayer = -1;
	for(i=0;i<MX_USERS;i++)
	{
		strcpy(send_buf,"");
		if(FD_ISSET(user_list[i].socket,clients))
		{
			if(user_list[i].score < threshold_score[user_list[i].level -1 ])
			{
				strcpy(send_buf,":( You did not qualify for next round. \n *************************** GAME OVER *****************************\n");
				send(user_list[i].socket,send_buf,1024,0);
				FD_CLR(user_list[i].socket,clients);
				
			}	
			else
			{
				strcpy(send_buf,"You qualify for next round.\nDo you want to continue to next level or quit the game?? [0: QUIT / 1: CONTINUE]\n");
				send(user_list[i].socket,send_buf,1024,0);
				recv(user_list[i].socket,recv_buf,1024,0);
				quit=atoi(recv_buf);
				if(quit==0)
				{
					strcpy(send_buf,":( ********************** GAME OVER ***********************\n");
					send(user_list[i].socket, send_buf,1024,0);
					FD_CLR(user_list[i].socket,clients);
				}
				else
				{
					user_list[i].score=0;
					user_list[i].level=user_list[i].level+1;
					cnt++;
					lastActivePlayer = i;
					//gameover=0;
				}
			}
		}
	}
	strcpy(send_buf,"");
	if(cnt == 1)
	{
		sprintf(send_buf,":( No other player left to play with.\n %s WON \n ********************** GAME OVER ***********************\n", user_list[lastActivePlayer].userName);
		send(user_list[lastActivePlayer].socket, send_buf,1024,0);		

	}
	else if(cnt > 1)
	{
		for(i=0;i<MX_USERS;i++)
		{
		
			if(FD_ISSET(user_list[i].socket,clients))
			{
				strcpy(send_buf,"****Going to next level ***\n");
				send(user_list[i].socket, send_buf,1024,0);			
			}
		}
		gameover = 0;
	}
	return gameover;
}


int gt_nextLevel(fd_set* clients,int maxfd)
{
	int gameover = 0;
	printf("goto next level\n");
	int i,max_score=0,cur_userid=0, winnerId = 0;
	if(select(maxfd+1, NULL, clients, NULL, NULL) == -1)//may have to recalculate maxfd 
	{//here select will return with all the descriptors which are 
             //ready to write , all others have to miss this question
		perror("select");
		exit(1);
    }
	char send_buf[5000]="";
	switch(mode)
	{
		case 1 :for(i=0;i<MX_USERS;i++)
				{
					strcpy(send_buf,"");
					if(FD_ISSET(user_list[i].socket,clients))
					{
					 	sprintf(send_buf,"%s - %d points \n ********************** GAME OVER ***********************\n", user_list[i].userName, user_list[i].score);
						
						flagc=1;
				        signal(SIGALRM,handler_connectc);
				        alarm(5);

				        int check = 0;
				        char points_buf[5000];
				        printf(" now going to send score\n");
				        while(flagc)
				        {
				           // printf("inside\n");
				            if(send(user_list[i].socket,send_buf,5000,0 > 0))
				            {
				                check = 1;
				                break;
				            }

				        }

				        if(check == 0)
				        {
				            printf("Could not send scores.Exiting.");
				         //   exit(1);
				        }
				        //printf("check----------------%d\n", check);
						// printf("score %d\n",user_list[i].score );	
						// int x = snprintf(send_buf,5000,"%s - %d points \n ********************** GAME OVER ***********************", user_list[i].userName, user_list[i].score);
						// printf("x ============== %s\n", send_buf);
						// send(user_list[i].socket,send_buf,5000,0);
						//printf("goto next level points\n");
						//strcpy(send_buf, " points \n");
					}				
	
				}
				gameover = 1;
				break;
		case 2: for(i=0;i<MX_USERS;i++)
				{
					strcpy(send_buf,"");
					if(FD_ISSET(user_list[i].socket,clients))
					{
						
						sprintf(send_buf + strlen(send_buf),"%s - %d points \n", user_list[i].userName, user_list[i].score);
						//send(user_list[i].socket,send_buf,1024,0);
						if(user_list[i].visibility == 0)
						{
							send(user_list[i].socket,send_buf,5000,0);
							continue;
						}
						int j = 0;
						for(j=0;j<MX_USERS;j++)
						{
							if(i != j && FD_ISSET(user_list[j].socket,clients) && user_list[j].visibility == 1)
							{
								sprintf(send_buf+strlen(send_buf),"%s - %d points \n", user_list[j].userName, user_list[j].score);
								//send(user_list[i].socket,send_buf,1024,0);
							}		
						}
						send(user_list[i].socket,send_buf,5000,0);
					}		
	
				}
				gameover = foundWinner(clients, maxfd);
				if(gameover == 1)
				{
					return 1;
				}
				gameover=updatePlayers(clients, maxfd);
				
				break;
				


	}

	return gameover;	
}

int main()
{
	 int i = 0, j = 0;
	// for(i = 0; i <3; i++)
	// {
	// 	for(j = 1; j < 51; j++)
	// 	{
	// 		hp[i][j].valid = 1;
	// 		cs[i][j].valid = 1;
	// 		ca[i][j].valid = 1;
	// 	}
	// }
	FILE* fp;

	for(i=1;i < 4;i++ )//category
	{
		for(j=1;j<4;j++)//level
		{
			fp=fopen(fpQues[i-1][j-1].Qfile,"r");
			//printf("File opened\n");
			InitialiseQues(fp,j,i);
			printf("Questions initialised for category %d level %d \n",i,j);
			fclose(fp);
			fp=fopen(fpAns[i-1][j-1].Afile,"r");
			//printf("File  ans opened\n");
			InitialiseAns(fp,j,i);
			printf("Answers initialised for category %d level %d \n",i,j);
			fclose(fp);
		}
	}
    int serverSocket, newSocket; // listen on serversock, new connection on newSocket
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];// max len of IP address
    int rv;
   //    struct sockaddr_in serverAddr;
  	// serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  	// serverAddr.sin_family = AF_INET;
  	// serverAddr.sin_port = htons(3758);
  	//  serverAddr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
  	//   memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );
  	//   bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) );
	//******************************************************** Server socket programming *******************************************************
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;//IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if((rv = getaddrinfo(NULL,PORT, &hints, &servinfo)) != 0)
	{ //getting which IPv server supports- 0 success
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));//gai_strerror -if anu error than returns error code
        return 1;
    }

    //loop through all the result and bind to the first we can
    for(p = servinfo; p != NULL; p  = p->ai_next)
	{
        if((serverSocket = socket(p->ai_family, p->ai_socktype| SOCK_NONBLOCK, p->ai_protocol)) == -1)
		{
            perror("server : socket");
            continue;
        }

        if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
            perror("set sockopt");
            exit(1);
        }

        if(bind(serverSocket, p->ai_addr, p->ai_addrlen) == -1)
		{
            close(serverSocket);
            perror("server: bind");
            continue;
        }

        break;
    }

    if(p == NULL)
	{
        fprintf(stderr, "server:failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo);//all done with this structure

    
    		//printf("workingbefore\n");
    
    if( listen ( serverSocket,PENDING  ) ==0 )
    {
    		//printf("working\n");
      		printf ( " here	 Listening \n " );
    }
	    else
    	  printf("More than limit. Error!!  \n");
    //************************************************************register clients****************************************************************
    while(1)
    {
    	/*
		while ( listen ( serverSocket,PENDING  ) ==0 )
      		printf ( " Listening \n " );
	    else
    	  printf("More than limit. Error!!  \n");
	/*/
    	//printf("hello----------------------\n");
    	int i = 0, j = 0;
		for(i = 0; i <3; i++)
		{
			for(j = 1; j < 51; j++)
			{
				hp[i][j].valid = 1;
				cs[i][j].valid = 1;
				ca[i][j].valid = 1;
			}
		}
		
	    fd_set clients;
	    fd_set recvfds;
	    fd_set temp;
		FD_ZERO(&clients);
		FD_ZERO(&temp);
		//FD_SET(serverSocket, &clients);
		//int maxfd = serverSocket;
		
		int maxfd= INT_MIN;

		signal(SIGALRM,handler_firstcon);
		alarm(60);
		 flag=1;
		 firstcon=1;
		//int mode = 1;
		while(firstcon && user_id < 1)
		{	
		//	printf("waiting for others to connect %d\n", flag);
			addr_size = sizeof ( serverStorage );
	      	newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
			if(newSocket > 0)
			{
				//printf("accept\n");
				mode = register_fn(newSocket);
				user_id++;
				printf("%d user id connection accepted\n",user_id);
				FD_SET(newSocket, &clients);
				if(maxfd < newSocket)
				{
					maxfd = newSocket;
				}/*
				if(mode == 1)// SINGLEPLAYER
				{
					break;
				}*/
				printf("First user connected ---\n");
			}
			
			/*
			else
			{
				printf("nah\n");
				break;
			}
			*/
		}

		// addr_size = sizeof ( serverStorage );
	 //  	newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
		// if(newSocket != -1)
		// {
		// 	printf("inside register\n");
		// 	mode = register_fn(newSocket);
		// 	user_id++;
		// 	FD_SET(newSocket, &clients);
		// 	if(maxfd < newSocket)
		// 	{
		// 		maxfd = newSocket;
		// 	}/*
		// 	if(mode == 1)// SINGLEPLAYER
		// 	{
		// 		break;
		// 	}*/
		// }
		
		//long on = 1L;
		signal(SIGALRM,handler_sa);
		alarm(WAIT_LTIME);
		 flag=1;
		//int mode = 1;
		while(flag && (user_id < 4))
		{	
			//printf("waiting for others to connect %d\n", flag);
			addr_size = sizeof ( serverStorage );
	      	newSocket = accept ( serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
			if(newSocket > 0)
			{
				//printf("accept\n");
				mode = register_fn(newSocket);
				user_id++;
				printf("%d user id connection accepted\n",user_id);
				FD_SET(newSocket, &clients);
				if(maxfd < newSocket)
				{
					maxfd = newSocket;
				}/*
				if(mode == 1)// SINGLEPLAYER
				{
					break;
				}*/
			}
			/*
			else
			{
				printf("nah\n");
				break;
			}
			*/

		}
		
		char start_buf[50];
		int st =0;
		for(st=0;st<MX_USERS;st++)
		{
			strcpy(start_buf,"****Starting Quiz****\n");
			if(FD_ISSET(user_list[st].socket,&clients))
			{
				send(user_list[st].socket,start_buf,50,0);
			}
		}
		printf("Starting  Quiz\n");
		//*************************************************************** Start quiz ************************************************************
		int i_Q = 0; 
		//char ques_buf[1024];//malloc
		for(i_Q = 1; i_Q <= mx_ques; i_Q++)
		{
			temp = clients;
			if(select(maxfd+1, NULL, &temp, NULL, NULL) == -1)//may have to recalculate maxfd 
			{//here select will return with all the descriptors which are 
	                 //ready to write , all others have to miss this question
				perror("select");
				exit(1);
	        }
		//*************************************************************** Send question ************************************************************
	    	
	        printf("Sending Questions\n");
			int Qno=sendQues(temp,maxfd,i_Q);
			/*
			srand(time(NULL));
			int Qno=rand()%51;
			int i;
			

			for(i=0;i<=maxfd;i++)
			{
				if(FD_ISSET(i,&temp))
				{
				 	switch(category)
					{
						case 1: strcpy(ques_buf,hp[user_list[0].level][Qno]);
								break;
						case 2: strcpy(ques_buf,cs[user_list[0].level][Qno]);
								break;
						case 3: strcpy(ques_buf,ca[user_list[0].level][Qno]);
								break;

					}
				    if(send( i, ques_buf, 1024, 0 )!=1024)
					{
						printf("Error while sending question to %d ",i);
					}
				}
			}*/	
		//************************************************************** receive answers *************************************************************
			//printf("will recv ans\n");
			recvAns(&clients,maxfd,Qno);
			printf("Answers received for Question.%d\n", i_Q);
			/*
			flag=1;		
			recvfds=clients;
			char ans_buf[10];
			FD_ZERO(&temp);
			signal(SIGALRM,handler_sa);
			alarm(WAIT_ATIME);//while loop ends after alarm goes off as flag becomes 0
			while(flag)//server waits to receive ans from clients for only wait_atime ...timer starts after sending question to all clients
			{	temp=recvfds;
				if(select(maxfd+1, &temp, NULL, NULL, NULL) <=0)
				{
	                perror("select");
	                //exit(4);
	                break;//break is important. Explained above
	            }

	            for(i = 0; i <= maxfd; i++) 
				{
	                //printf("Recving answer I(%d)\n",i);
	                if(FD_ISSET(i, &temp)) 
					{
						if(recv(i,ans_buf,10,0)==0)//check conditions for all 0/-1 etc
						{
							printf("error.Closing connection with %d\n",i);
							FD_CLR(i,&temp);
							FD_CLR(i,&recvfds);
							FD_CLR(i,&clients);	
						}
						else
						{
							FD_CLR(i,&temp);
							FD_CLR(i,&recvfds);	
						}
						

				}
			}

			*/

			//************************************************************ Quit or Continue ***********************************************************
			if(i_Q == mx_ques)
			{
				int gameover = gt_nextLevel(&clients,maxfd);
				if(gameover==0)
				{
					i_Q=0;
					continue;
				}
				
			}
			

		}
		//printf("ending----------------\n");
	}
	close(serverSocket);
    return 0;

}
