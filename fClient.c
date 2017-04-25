/*
 *  Description: Client side file for players to play games.
*/

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
#include <time.h>

#define TRUE   1 
#define FALSE  0 
#define PORT 3758

#define CONN_SETUP 2
#define CONN_SETUP1 2
#define RECV_Q 12
#define WAIT_ANS 10
//#define Waiting_ans 8

int flag=1, flag2 = 1, flag4 = 1, flaga = 1, flagb = 1, flagc = 1,flag_st = 1;

void handler_sa()
{
    flag=0;
}
void handler_answer()
{
    flag2=0;
}
void handler_sending()
{
    flag4=0;
}
void handler_connect()
{
    flaga=0;
}
void handler_connect1()
{
    flagb=0;
}

void handler_connectc()
{
    flagc=0;
}
void handler_st()
{
    flag_st=0;
}
// void handler_ans()
// {
//     signal(SIGALRM,handler_sa);
//     alarm(CONN_SETUP);
//     flag=1;
//     strcpy(send_buf,"Not answered");
//     while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) != sizeof(send_buf) && flag) 
//     {
//             printf("[ERROR] Unable to send username.Retrying\n");
//             // tries++;
//     }
//     if(flag==0)
//     {
//         printf("Could not send ans.Exiting.");
//         exit(1);
//     }
// }
// void fflushstdin()
// {
//     //strcpy(ans, "");
//     int c;
//     while( (c = fgetc( stdin )) != EOF && c != '\n' );
// }
int encrypt_fn(char c)
{
    //int val = atoi(c);
  //  char e_ans[10];
    int e_val = (c+0)*2 - 11;
    return e_val;
}
int main(int argc,char* argv[])
{
    int tries=0;

    char send_buf[1024],recv_buf[1024];
    // Important declaration
    int clientSocket;
    //char *some_message;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    char ip_address[50];
    strcpy(ip_address,"127.0.0.1");
    // User Input
    // char *userInput;

    // userInput = (char *)malloc(sizeof(char)*10);
    // some_message = (char *)malloc(sizeof(char)*10);

    /*  Create the network socket.   */
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket<0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /*  1) PR_INET Internet domain
        2) SOCK_STREAM represents Stream socket
        3) 0 represents Default protocol (TCP in this case)
        */

    // Configure settings of the server address
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;

    /* Set port number */
    serverAddr.sin_port = htons(PORT);
    /*  In networking we prefer to use big-endian binary 
        sequencing.  Since in our PC we use small endian binary
        scheme, we use htons to convert little endian to big
        endian.  
        */

    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr(ip_address);//Change this IP address to static address

    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof
            serverAddr.sin_zero);

    /* Connect the socket to the server using the address*/


    /***********************************************Connection and user details******************************************************/

    addr_size = sizeof serverAddr;
    if ( connect( clientSocket, (struct sockaddr *) &serverAddr, 
                addr_size) < 0 )
    {
        printf("[ERROR] Unable to connect to server.\n");
        close( clientSocket );
        exit(0);
    }
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    flaga=1;
    int check=0;
    // while( recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) < 0  && flag)//uername
    // {
    //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //     // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Connection never got established.Exiting.");
    //     exit(1);
    // }
    while(flaga)
    {
        if(recv ( clientSocket, recv_buf, sizeof(recv_buf), 0)!= 0)// == sizeof(recv_buf))//uername
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
        printf("Connection never got established.Exiting.");
       exit(1);
    }

    tries=0;
    flag=1;
    printf("%s",recv_buf);//username
    scanf("%s",send_buf);
    //printf("you entered %s\n",send_buf);
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    flaga =1;
    // while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) < 0 && flag) 
    // {
    //         printf("[ERROR] Unable to send username.Retrying\n");
    //         // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Could not send username.Exiting.");
    //     exit(1);
    // }
    check=0;
    while(flaga)
    {
        if( send ( clientSocket, send_buf, sizeof(send_buf), 0)!= 0) // == sizeof(send_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
            printf("Could not send username.Exiting.");
         exit(1);
    }
    tries=0;
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    flag=1;
    flaga =1;
    check=0;
    //printf("getting in while1\n");
    // while( recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) < 0 && flag)//modes
    // {
    //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //     // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Got Disconnected.Exiting.");
    //     exit(1);
    // }
    while(flaga)
    {
        if(recv ( clientSocket, recv_buf, sizeof(recv_buf), 0)!= 0) // == sizeof(recv_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
        printf("Got Disconnected.Exiting.");
        exit(1);
    }
    //printf("getting out while1 %d\n", flag);
    tries=0;
   
    printf("%s",recv_buf);
    scanf("%s",send_buf);
    int mode = atoi(send_buf);
    //printf("you entered %s\n",send_buf);
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    flag=1;
    flaga =1;
    check=0;
    // while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) < 0 && flag) 
    // {
    //         printf("[ERROR] Unable to send mode type.retyring.\n");
    //         // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Could not send mode type.Exiting.");
    //     exit(1);
    // }
    //return 0;
    while(flaga)
    {
        if(send (clientSocket, send_buf, sizeof(send_buf), 0) != 0) //== sizeof(send_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
            printf("Could not send mode type.Exiting.");
            exit(1);
    }
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    tries=0;
    flag=1;
    flaga =1;
    check=0;
    //printf("going in while of category\n");
    // while( recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) < 0  && flag)//category
    // {
    //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //     // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Got Disconnected.Exiting.");
    //     exit(1);
    // }
    //printf("going out while of category\n");
    
    while(flaga)
    {
        if(recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) != 0) // == sizeof(recv_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
            printf("Got Disconnected.Exiting.");
         exit(1);
    }
    printf("%s",recv_buf);
    scanf("%s",send_buf);
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    tries=0;
    flag=1;
    flaga =1;
    // while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) != sizeof(send_buf) && flag) 
    // {
    //         printf("[ERROR] Unable to send category type.retyring.\n");
    //         // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Could not send category type.Exiting.");
    //     exit(1);
    // }
    while(flaga)
    {
        if(send (clientSocket, send_buf, sizeof(send_buf), 0)!= 0) // == sizeof(send_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
         printf("Could not send category type.Exiting.");
         exit(1);
    }

    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    tries=0;
    flag=1;
    flaga =1;
    check=0;

    // while( recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) < 0  && flag)
    // {
    //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //     // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Got Disconnected.Exiting.");
    //     exit(1);
    // }

    while(flaga)
    {
        if(recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) != 0) //== sizeof(recv_buf) )
        {
            check = 1;

            break;
        }

    }

    if(check == 0)
    {
        printf("Got Disconnected.Exiting.");
        exit(1);
    }

    printf("%s\n",recv_buf);
    scanf("%s",send_buf);
    int level = atoi(send_buf);
    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    tries=0;
    flag=1;
    flaga =1;
    check=0;
    // while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) != sizeof(send_buf) && flag) 
    // {
    //         printf("[ERROR] Unable to send level.retyring.\n");
    //         // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Could not send level.Exiting.");
    //     exit(1);
    // }
    while(flaga)
    {
        if(send ( clientSocket, send_buf, sizeof(send_buf), 0) == sizeof(send_buf) )
        {
            check = 1;
           break;
        }

    }

    if(check == 0)
    {
        printf("Could not send level.Exiting.");
        exit(1);
    }

    signal(SIGALRM,handler_connect);
    alarm(CONN_SETUP);
    tries=0;
    flag=1;
    flaga=1;
    check =0;
    // while( recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) < 0  && flag)
    // {
    //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //     // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Got Disconnected.Exiting.");
    //     exit(1);
    // }
    while(flaga)
    {
        if(recv ( clientSocket, recv_buf, sizeof(recv_buf), 0) == sizeof(recv_buf) )
        {
            check = 1;
            break;
        }

    }

    if(check == 0)
    {
       printf("Got Disconnected.Exiting.");
       exit(1);
            // tries++;
    }
    printf("%s\n",recv_buf);
    scanf("%s",send_buf);

    signal(SIGALRM,handler_connect1);
    alarm(CONN_SETUP1);
    tries=0;
    flag=1;
    flagb =1;
    check =0;
    // while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) != sizeof(send_buf) && flag) 
    // {
    //      printf("[ERROR] Unable to send visibility settings.retyring.\n");
    //         // tries++;
    // }
    // if(flag==0)
    // {
    //     printf("Could not send level.Exiting.");
    //     exit(1);
    // }

    while(flagb)
    {
        if(send ( clientSocket, send_buf, sizeof(send_buf), 0) == sizeof(send_buf)  )
        {
            check = 1;
            break;
        }

    }

    if(check == 0)
    {
        printf("Could not send visibility.Exiting.");
        exit(1);
    }
    /**************************************************************User Setup done**************************************************/


    /**************************************************************Game Initiation**************************************************/
    //printf("*************Game is starting**************\n");
    printf("Waiting for other users to connect .........\n");
    printf("Enter your answer as A, B, C or D. If you want to pass, answer NA. \n");


    // int game_over=0;
    //sleep(11);
    signal(SIGALRM,handler_st);
    alarm(11);
    char start_buf[50];
    flag_st=1;
    while(flag_st)
    {
        if(recv( clientSocket, start_buf, sizeof(start_buf), 0) == sizeof(start_buf))
        {
            if(start_buf[0]=='*')
            {
                flag_st = 0;
            }
        }
        printf("%s",start_buf);
    }
    
    int count=1;
    int is_zero=0;
    char ques[5000];
    char ans[10];
    int flag1=1;

    while(flag1)
    {
        LOOP: for(count=1;count<=10;count++)
        {   
            //printf("loop count %d\n",count);
            signal(SIGALRM,handler_sa);
            alarm(RECV_Q);
            flag=1;
            int check =0;
            // fflush(stdin);
            // // fflush(stdin);
            // fflush(stdin);
            // //fflush(ans);
            // fflushstdin();
            strcpy(ans, "");
            while(flag)
            {
                if(recv( clientSocket, ques, sizeof(ques), 0) == sizeof(ques)  )
                {
                   // printf("recv buf %s\n", ques);
                    check = 1;
                    break;
                }

            }

            if(check == 0)
            {
                printf("Could not recv question.Exiting.\n");
                exit(1);
            }
            // while( recv ( clientSocket, ques, sizeof(ques), 0) < 0  && flag)
            // {
            //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
            //     // tries++;
            // }
            // if(flag==0)
            // {
            //     printf("Question not received. Closing");
            //     exit(1);
            // }
            //printf("received %s\n",ques);
            int x;
            char subbuff[3];
            if(ques[2]=='.')
            {
                subbuff[0]=ques[1];
                subbuff[1]='\0';
                x=atoi(subbuff);
            }
            else if(ques[3]=='.')
            {
                subbuff[0]=ques[1];
                subbuff[1]=ques[2];
                subbuff[2]='\0';
                x=atoi(subbuff);
            }
            else
            {
                //printf("%s\n", );
                flag1=0;
                is_zero=1;
                break;
            }
            if(x!=count)
            {
                flag1=0;
                is_zero=1;
                break;
            }
            printf("%s\n",ques);
            // signal(SIGALRM,handler_ans);
            // alarm(WAIT_ANS);
            //sleep(12);
            signal(SIGALRM,handler_answer);
            alarm(WAIT_ANS);
            flag2=1;
            int didAns = 0;
            while(flag2)
            {
                //printf("scan\n");
                //if(scanf("%s",ans) != -1 || scanf("%s, ans") != EOF ||scanf("%s, ans") != NULL)
                
               // if(gets(ans) != EOF || gets(ans) != NULL)
                //fflush(stdin);
                // strcpy(ans, "");
                // fflushstdin();
                // fseek(stdin, 0, SEEK_END);
                if(scanf("%s", ans) == 1)
                {
                   // fflush(stdin);
                    didAns = 1;
                    signal(SIGALRM,handler_sending);
                    alarm(CONN_SETUP);
                    flag4=1;
                    int naAns = 0;
                    char e_ans[10];
                    int e_val = encrypt_fn(ans[0]);
                    //strcpy(e_ans,encrypt_fn(ans));
                    sprintf(e_ans, "%d", e_val);
                    //printf("e_ans = %s\n", e_ans);
                    // while ( send ( clientSocket, ans, sizeof(ans), 0) != sizeof(ans) && flag4) 
                    // {
                    //         printf("[ERROR] Unable to send ANS.Retrying\n");
                    //         // tries++;
                    // }
                    // if(flag4==0)
                    // {
                    //     printf("Could not send ans.Exiting.");
                    //     exit(1);
                    // }
                    //  while(getchar() == '\n')
                    // {
                        
                    // }
                    // while(getchar()!= '\n')
                    // {

                    // }
                    while(flag4)
                    {
                        if(send ( clientSocket, e_ans, sizeof(e_ans), 0) == sizeof(e_ans) )
                        {
                            naAns = 1;

                            break;
                        }

                    }


                    if(naAns == 0)
                    {
                            printf("[ERROR] Unable to send ANS.\n");
                            // tries++;
                    }
                    break;
                }
            
            }

         
            // if(didAns == 0)
            // {
            //     printf("You did not answer\n");
            //     signal(SIGALRM,handler_sending);
            //     alarm(CONN_SETUP);
            //     flag4=1;
            //     int naAns = 0;
            //     strcpy(send_buf,"NA");
            //     while(flag4)
            //     {
            //         printf("NAWhile\n");
            //         if(send ( clientSocket, send_buf, sizeof(send_buf), 0) == sizeof(send_buf))
            //         {
            //             printf("You did not answer sending NA\n");
            //             naAns = 1;

            //             break;
            //         }

            //     }
            //     if(naAns == 0)
            //     {
            //             printf("[ERROR] Unable to send signal that not answered.\n");
            //             // tries++;
            //     }
            //     // if(flag4==0)
            //     // {
            //     //     printf("Could not send ans.Exiting.");
            //     //     exit(1);
            //     // }
            // }

        }
        flagc=1;
        signal(SIGALRM,handler_connectc);
        alarm(13);

        int check = 0;
        char points_buf[5000];
        //printf(" now going to recieve score\n");
         while(flagc)
        {
            //printf("inside\n");
            if(recv ( clientSocket, points_buf, sizeof(points_buf), 0) > 0)
            {
                check = 1;
                break;
            }

        }

        if(check == 0)
        {
            printf("Could not rec scores.Exiting.");
            exit(1);
        }
        //while( recv ( clientSocket, points_buf, sizeof(points_buf), 0) != sizeof(points_buf))//  && flag)
        // {
        //     printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
        // }
        printf("%s",points_buf );//points
        // End game if single player mode
        if(mode == 1)
        {
            exit(0);
        }

        //Print winner if level is 3 and mode is multiplayer

        if(level == 3 && mode == 2)
        {    
            char win_buf[1024];
            flag=1;
            signal(SIGALRM,handler_sa);
            alarm(CONN_SETUP);
            while(flag)
            {
                //printf("inside\n");
                if(recv ( clientSocket, win_buf, sizeof(win_buf), 0) > 0)
                {
                    check = 1;
                    break;
                }

            }

            if(check == 0)
            {
                printf("Could not rec winner.Exiting.");
                exit(1);
            }
            printf("%s",win_buf );//winner
        }


        //Update player to next level



        char updt_buf[1024];
        flaga =1;
        signal(SIGALRM,handler_connect);
        alarm(CONN_SETUP);
        while(flaga)
        {
            //printf("inside\n");
            if(recv ( clientSocket, updt_buf, sizeof(updt_buf), 0) > 0)
            {
                check = 1;
                break;
            }

        }

        if(check == 0)
        {
            printf("Could not recv info of next level.Exiting.");
            exit(1);
        }
        printf("%s",updt_buf);// Qualification----------------
        
        if(updt_buf[0]==':')
        {
            exit(0);// Exit if player did not qualify for next round----------------
        }

        signal(SIGALRM,handler_answer);
        alarm(WAIT_ANS);
        flag2=1;
        char quit[10];
        int didAns = 0;
        while(flag2)
        {
           
            if(scanf("%s", quit) == 1)
            {
                didAns = 1;
                signal(SIGALRM,handler_sending);
                alarm(CONN_SETUP);
                flag4=1;
                int naAns = 0;
              
                while(flag4)
                {
                    if(send ( clientSocket, quit, sizeof(quit), 0) == sizeof(quit) )
                    {
                        naAns = 1;

                        break;
                    }

                }

                if(naAns == 0)
                {
                        printf("[ERROR] Unable to send choice .\n");
                        // tries++;
                }
                break;
            }
        
        }

        if(quit[0]== '0')// User quits-----
        {

            strcpy(updt_buf,"");
            flaga =1;
            signal(SIGALRM,handler_connect);
            alarm(CONN_SETUP);
            while(flaga)
            {
               // printf("inside\n");
                if(recv ( clientSocket, updt_buf, sizeof(updt_buf), 0) > 0)// recv game over--------
                {
                    check = 1;
                    break;
                }

            }

            if(check == 0)
            {
                printf("Could not recv info of gameover.Exiting.");
                exit(1);
            }
            printf("%s",updt_buf );//print gameover-----
            exit(0);
        }

        else// user wants to play----------------------
        {
             strcpy(updt_buf,"");
            flaga =1;
            signal(SIGALRM,handler_connect);
            alarm(CONN_SETUP);
            while(flaga)
            {
               // printf("inside\n");
                if(recv ( clientSocket, updt_buf, sizeof(updt_buf), 0) > 0)// recv game over--------
                {
                    check = 1;
                    break;
                }

            }

            if(check == 0)
            {
                printf("Could not recv info of gameover.Exiting.");
                exit(1);
            }
            printf("%s",updt_buf );//1. no other player left.gameover //2.going to next level
            if(updt_buf[0]== ':')
            {
                exit(0);
            }
            else 
            {
                goto LOOP;
            }

        }


    }


    //     // todi-----------------------------
    //     while( recv ( clientSocket, _buf, sizeof(win_buf), 0) < 0)//  && flag)
    //         {
    //             printf("[ERROR] Connection closed prematurely.Trying to connect again\n");
    //         }
    //     if(strcmp(points_buf,"Do you want to continue to next level or quit the game?? [0: QUIT / 1: CONTINUE]")==0)
    //     {
    //         scanf("%s",send_buf);
    //         while ( send ( clientSocket, send_buf, sizeof(send_buf), 0) != sizeof(send_buf) && flag) 
    //             {
    //                 printf("[ERROR] Unable to send username.Retrying\n");
    //                     // tries++;
    //             }
    //             if(flag==0)
    //             {
    //                 printf("Could not send response.Exiting.");
    //                 exit(1);
    //             }
    //         if(strcmp(send_buf,"0")==0)
    //         {
    //             flag1=0;
    //             is_zero=2;
    //             break;
    //         }
    //         else
    //             goto LOOP;
    //     }
    //     else
    //     {
    //         flag1=0;
    //         break;
    //     }
    // }
    
    // if(is_zero==1)
    // {
    //     printf("There was error in connection\n");
    // }
    // else if(is_zero==2)
    // {
    //     printf("You withdrew from game\n");
    // }
    // else
    // {
    //     printf("%s",recv_buf);
    // }
    close( clientSocket );
    return 0;
}