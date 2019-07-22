#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#define Base_URL "https://www.chitkara.edu.in"
#define urlLength 200
int file_no=1;
struct Link
{
	char url[200];
	int depth;
	int isVisited;
	int key;
	struct Link *next;
};
struct hash
{
	struct Link *head;
};


int GetNextURL(char* html, char* urlofthispage, char* result, int pos) ;
int hashFunction(char *s);
struct Link * createLinkList(struct Link*,struct Link *,struct hash*);
struct Link * extractLinks(char *str,int depth,char *url);
struct Link * convertArray(char **links,int depth,int l);

int checkUrl(char url[])
{
	char v[]="wget --spider ";
	char *b=(char *)malloc(sizeof(char)*300);
	strcpy(b,v);
	char ch[]="https://";
	char c[100];
	strcpy(c,ch);
	int k=8;
	for(int i=8;url[i]!='/';i++)
	{
		c[k]=url[i];
		k++;
	}
	c[k]='\0';
	strcat(b,c);
	if(!system(b))
	{
		if(strcmp(Base_URL,c))
		{
			printf("URL is not correct");
			exit(0);
		}
		else
			return 1;
	}
}
int checkDepth(int depth)
{
	if(depth<0)
	{
		printf("Depth can't be negative");
		exit(0);
	}
	if(depth>5)
	{
		printf("Depth too high");
		exit(0);
	}
	return 1;
}
int checkDir(char *name)
{
	struct stat statbuf;
	if(stat(name,&statbuf)==-1)
	{
		char str[100]="mkdir ";
		strcat(str,name);
		system(str);
		return 1;
	}
	if(!S_ISDIR(statbuf.st_mode))
	{
		printf("Invalid directory entry.Your input isn't a directory\n");
		exit(0);
	}
	if((statbuf.st_mode & S_IWUSR)!=S_IWUSR)
	{
		printf("Invalid directory entry. It isn't writable\n");
		exit(0);
	}
	return 1;



}
void getPage(char *url)
{
	char urlBuffer[urlLength+300]={0};
	strcat(urlBuffer,"wget -O ");
	strcat(urlBuffer,"./temp.txt ");
	strcat(urlBuffer,url);
	system(urlBuffer);
	system("clear");
	printf("Page fetched successfully\n");
}

char* createFile(char *url,int depth,char *dir)
{
	FILE *ptr1,*ptr2;
	ptr2=fopen("temp.txt","r");
	fseek(ptr2, 0L, SEEK_END);
	int size = ftell(ptr2);
	fseek(ptr2, 0L, SEEK_SET);
	printf("%d\n",file_no);
	char itostring[10];
	sprintf(itostring,"%d",file_no);
	file_no++;
	char f[]=".html";
	char file[20]="";
	strcat(file,itostring);
	strcat(file,f);
	ptr1=fopen(file,"w");
	fprintf(ptr1,"%s",url);
	fprintf(ptr1,"%c",'\n');
	fprintf(ptr1,"%d",depth);
	fprintf(ptr1,"%c",'\n');
	
	char *str=(char *)malloc(size+1);
	int k=0;
	char c=fgetc(ptr2);
	str[k]=c;
	k++;
	while(c!=EOF)
	{
		fputc(c,ptr1);
		c=fgetc(ptr2);
		str[k]=c;
		k++;
	}
	str[k-1]='\0';
	fclose(ptr2);
	fclose(ptr1);
	
	ptr2=fopen("temp.txt","w");
	fclose(ptr2);
	char host[50],cwd[50];
	char a[]="mv ";
	strcat(a,file);
	strcat(a," ");
	getcwd(cwd,sizeof(cwd));
	strcat(a,cwd);
	strcat(a,"/");
	strcat(a,dir);
	system(a);
	return str;
}
struct Link* extractLinks(char *str,int depth,char *url)
{
	char **links=(char **)malloc(100*sizeof(char*));
	for(int i=0;i<100;i++)
	{
		links[i]=(char *)calloc(sizeof(char),200);      
	}
	int l=0,z=0;
	while(l<100)
	{
		char *result=(char*)calloc(sizeof(char),200);
		z=GetNextURL(str,url,result,z);
		if(z==-1)
			break;
		int flag=0;
		for(int j=0;j<l;j++)
		{
			if(strcmp(links[j],result)==0)
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
		{
			strcpy(links[l],result);
			l++;
		}
		free(result);
	}
	printf("%d\n",l);	
	return convertArray(links,depth,l);
}
struct Link * convertArray(char **links,int depth,int l)
{
	struct hash *h1=(struct hash*)calloc(sizeof(struct hash),10);
	struct Link *main1=(struct Link*)malloc(sizeof(struct Link)*1);
	main1=NULL;
	for(int i=0;i<l;i++)
	{
		int key=hashFunction(links[i]);
		struct Link *l1=(struct Link*)malloc(sizeof(struct Link)*1);
		strcpy(l1->url,links[i]);
		l1->depth=depth-1;
		if(i==0)
			l1->isVisited=1;
		else
			l1->isVisited=0;
		l1->key=key;
		l1->next=NULL;
		main1=createLinkList(l1,main1,h1);
	}
	return main1;
	/*struct Link *temp=main1;
	while(temp!=NULL)
	{		
		printf("%d %d %d %s\n",temp->key,temp->isVisited,temp->depth,temp->url);
		temp=temp->next;
	}*/
}

struct Link* createLinkList(struct Link* l,struct Link *main1,struct hash *h1)
{
	struct Link *newNode=(struct Link*)malloc(sizeof(struct Link)*1);
	strcpy(newNode->url,l->url);
	newNode->depth=l->depth;
	newNode->isVisited=l->isVisited;
	newNode->key=l->key;
	newNode->next=NULL;	
	if(main1==NULL)
		main1=newNode;
	else
	{
		struct Link *temp=main1;
		struct Link *prev=NULL;	
		if(h1[l->key].head==NULL)
		{
			h1[l->key].head=newNode;
			while(temp->next!=NULL)
			{
				temp=temp->next;
			}
			temp->next=newNode;
		}
		else
		{
			temp=h1[l->key].head;
			int flag=0;
			while(temp!=NULL)
			{		
				if(temp->key==newNode->key)
				{
					prev=temp;
					temp=temp->next;
				}
				else
				{
					newNode->next=temp;
					prev->next=newNode;
					flag=1;
					break;
				}
			}
			if(flag==0)
				prev->next=newNode;
		}
	}
	return main1;
}

int hashFunction(char *s)
{
	int l=strlen(s);
	int key=0;
	for(int i=0;i<l;i++)
		key+=s[i];
	while(key>=10)
	{
		key=key/10;
	}
	return key;
}	

void removeWhiteSpace(char* html) 
{
  int i;
  char *buffer = malloc(strlen(html)+1), *p=malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++) 
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}

int GetNextURL(char* html, char* urlofthispage, char* result, int pos) 
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) 
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) 
  {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');
    
    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;    

    p2 = strpbrk(p1, "\'\">");
    if (!p2) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#') 
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4)) 
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
	// again...probably good to recursively keep going..
	// NEW
        
        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }    
  return -1;
}


int main(int argc,char *argv[])
{
	char *url=(char*)malloc(strlen(argv[1])+1);
	strcpy(url,argv[1]);
	char *depth=(char*)malloc(strlen(argv[2])+1);
	strcpy(depth,argv[2]);
	int d;
	sscanf(depth,"%d",&d);
	char *dir=(char*)malloc(strlen(argv[3])+1);
	strcpy(dir,argv[3]);
	int a=checkUrl(url);
	int b=checkDepth(d);
	int c=checkDir(dir);
	if(a==1 && b==1 && c==1)
		printf("All Correct!\n");
	getPage(url);
	char *str=createFile(url,d,dir);
	struct Link *temp= extractLinks(str,d,url);
	while(temp!=NULL)
	{		
		printf("%s\n",temp->url);
		if(temp->isVisited==0)
		{	
			getPage(temp->url);
			char *s=createFile(temp->url,temp->depth,dir);
			struct Link *new= extractLinks(s,temp->depth,temp->url);
			free(new);
			temp->isVisited=1;
		}
		temp=temp->next;
	}
	
	
}
