#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdbool.h>

struct page
{
	int pageId;
	int titleCount;
	char *link;
	struct page *next;
};
struct word
{
	char *word;
	struct word *next;
	struct page *head;
};
struct hash
{
	struct word* head;
}h[26];
struct word* main1=NULL;
struct word * addToLinkList(char *str,int si,int ei,struct word *main1,int pageId);
void extractWords(char *str,int pageId)
{
	bool inWord=false;
	int i=0,si=-1,ei=-1;
	while(str[i]!='\0')
	{
		if(str[i]==' ')
		{
			if(inWord==true)
				ei=i-1;
			inWord=false;
		}
		else
		{
			if(inWord==false)
				si=i;
			inWord=true;
		}
		if(si!=-1 && ei!=-1)
		{
			main1=addToLinkList(str,si,ei,main1,pageId);
			si=-1;ei=-1;
		}
		i++;
	}
	if(inWord==true)
	{
		ei=i-1;
		main1=addToLinkList(str,si,ei,main1,pageId);
	}
	
}

struct word* addToLinkList(char *str,int si,int ei,struct word *main1,int pageId)
{
	char a[ei-si+2];
	int k=0;
	for(int i=si;i<=ei;i++)
	{
		if(str[i]>=65 && str[i]<=90)
			str[i]+=32;
		if(str[i]>=97 && str[i]<=122)
		{
		a[k]=str[i];
		k++;
		}
	}
	if(k==0)
		return main1;
	a[k]='\0';
	//printf("%s\n",a);
	struct word *newNode=(struct word *)malloc(sizeof(struct word)*1);
	newNode->word=(char*)malloc(strlen(a)*sizeof(char));
	strcpy(newNode->word,a);
	newNode->next=NULL;
	newNode->head=NULL;
	FILE *fptr;
	char page[40];
	char cwd[30];
	getcwd(cwd, sizeof(cwd));
	strcat(cwd,"/Links/");
	strcpy(page,cwd);
	char x[3];
	sprintf(x,"%d",pageId);
	strcat(page,x);
	strcat(page,".html");
	fptr=fopen(page,"r");
	char str1[200];
	int l=0;
	char c=fgetc(fptr);
	str1[l]=c;
	l++;
	while(c!='\n')
	{
		c=fgetc(fptr);
		str1[l]=c;
		l++;
	}
	str1[l-1]='\0';
	fclose(fptr);
	//printf("%s\n",str1);
	struct page *newNode1=(struct page *)malloc(sizeof(struct page)*1);
	newNode1->pageId=pageId;
	newNode1->titleCount=1;
	newNode1->link=(char *)malloc(l*1);
	strcpy(newNode1->link,str1);
	newNode1->next=NULL;
	if(main1==NULL)
	{
		newNode->head=newNode1;
		main1=newNode;
		h[a[0]-97].head=main1;
	}
	else
	{
		struct word *temp=main1;
		struct word *prev=NULL;
		if(h[a[0]-97].head==NULL)
		{
			newNode->head=newNode1;
			h[a[0]-97].head=newNode;
			while(temp->next!=NULL)
				temp=temp->next;
			temp->next=newNode;
		}
		else
		{
			temp=h[a[0]-97].head;
			int flag=0;
			while(temp!=NULL)
			{
				if(strcmp(temp->word,newNode->word)==0)
				{
					struct page *temp1=temp->head;
					int flag1=0;
					struct page *prev1=NULL;
					while(temp1!=NULL)
					{
						if(temp1->pageId==pageId)
						{
							temp1->titleCount++;
							flag1=1;
							break;
						}
						prev1=temp1;
						temp1=temp1->next;
					}
					if(flag1==0)
						prev1->next=newNode1;
					return main1;
				}
				if(temp->word[0]==newNode->word[0])
				{
					prev=temp;
					temp=temp->next;
				}
				else
				{
					//printf("%s %s\n",prev->word,newNode->word);
					newNode->head=newNode1;
					newNode->next=temp;
					prev->next=newNode;
					flag=1;
					break;
				}
			}
			if(flag==0)
			{
				newNode->head=newNode1;
				prev->next=newNode;
			}
		}
	}
	return main1;
}
char * convertFileToString(char *fileName)
{
	//printf("%s\n",fileName);
	struct stat st;
	stat(fileName,&st);
	int size=st.st_size;
	char *str=(char *)calloc(sizeof(char),size);
	FILE *ptr;
	
	ptr=fopen(fileName,"r");
	char c=fgetc(ptr);
	int k=0;
	str[k]=c;
	k++;
	while(c!=EOF)
	{
		c=fgetc(ptr);
		str[k]=c;
		k++;
	}
	str[k-1]='\0';
	fclose(ptr);
	return str;
}

void findTitle(char *str,int pageId)
{
	char *start,*end;
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]=='<' && str[i+1]=='t')
		{
			start = &str[i];
            		start = strpbrk(start, ">");
            		start++;
            		break;
		}
	}
	end=strpbrk(start,"<");
	int size=end-start;
	char title[size+1];	
	int k=0;
	for(char *i=start;i!=end;i++)
	{
		title[k]=*i;
		k++;
	}
	title[k]='\0';
	extractWords(title,pageId);
}

/*void findMeta(char *str)
{
	printf("META:\n");
	char *start,*end;
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]=='<' && str[i+1]=='m')
		{
			for(int j=i;j<strlen(str);j++)
				if(str[j]=='c' && str[j+1]=='o' && str[j+2]=='n')
				{	start = &str[j];
					start=strpbrk(start,"\"");
					start++;
					break;
				}
       			end=strpbrk(start,"\"");
			for(char *j=start;j!=end;j++)
				printf("%c",*j);
			printf("\n");
		}
	}
	
}*/

/*void findHeadings(char *str)
{
	printf("HEADINGS:\n");
	char *start,*end;
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]=='<' && str[i+1]=='h' &&(str[i+2]=='1' ||str[i+2]=='2' ||str[i+2]=='3' ||str[i+2]=='4' ||str[i+2]=='5' ||str[i+2]=='6'))
		{
			start = &str[i];
            		start = strpbrk(start, ">");
            		start++;
           		end=strpbrk(start,"<");
			for(char *j=start;j!=end;j++)
				printf("%c",*j);
			printf("\n");
		}
	}
	
}*/

void save(struct word *t)
{
	FILE *fptr=fopen("savelist.txt","w");
	while(t!=NULL)
	{
		fprintf(fptr,"%s ",t->word);
		struct page *temp=t->head;
		while(temp!=NULL)
		{
			fprintf(fptr,"%d %d ",temp->pageId,temp->titleCount);
			if(temp->next==NULL)
				fprintf(fptr,"%s\n",temp->link);
			else
				fprintf(fptr,"%s ",temp->link);
			temp=temp->next;
		}
		t=t->next;
	}
	fclose(fptr);
}

int main()
{
	FILE *ptr2;
	ptr2=fopen("savelist.txt","r");
	fseek(ptr2, 0L, SEEK_END);
	int size = ftell(ptr2);
	fseek(ptr2, 0L, SEEK_SET);
	for(int i=0;i<26;i++)
		h[i].head=NULL;
	char fileName[40];
	for(int i=1;i<=10;i++)
	{
		char cwd[30];
		getcwd(cwd, sizeof(cwd));
		strcat(cwd,"/Links/");
		strcpy(fileName,cwd);
		char x[3];

		sprintf(x,"%d",i);
		strcat(fileName,x);
		strcat(fileName,".html");
		//printf("%s\n",fileName);
		char *str=convertFileToString(fileName);
		findTitle(str,i);
		//findHeadings(str);
		//findMeta(str);
	}
	struct word *t=main1;
	while(t!=NULL)
	{
		printf("%s\n",t->word);
		struct page *temp=t->head;
		while(temp!=NULL)
		{
			printf("%d %d %s\n",temp->pageId,temp->titleCount,temp->link);
			temp=temp->next;
		}
		t=t->next;
	}	
	save(main1);
}
