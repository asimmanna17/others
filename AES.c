#include<stdio.h>
#include<stdlib.h>

int sbox[16][16];

char *str, *key;

int state[4][4], Key[4][4];

int mulCol[4][4];

int fieldInv(int a)
{
	int j,i,b=0,mod=283;
	for (i=1;i<256;i++)
	{
		b=0;
		for(j=0;j<8;j++)
		{
			int temp=(a>>j)&1;
			if(temp==1)
				b=b^(i<<j);	
		}
		int h=(b>>8);
		while(h>0)
		{
			int l=8,temp=(b>>l)&1;
			while(temp!=1)
			{
				l++;
				temp=(b>>l)&1;
			}
			int temp1=(mod<<(l-8));
			b=b^temp1;
			h=(b>>8);
		}
		if (b==1)
			return i;
	}
	return -1;
}

int inv(int k)
{
	int c=0x63,i;
	if (k==0)
		return c;
	int a;
	a=fieldInv(k);
	int b=0;
	for (i=0;i<8;i++)
	{
		int b1,b2,b3,b4,b5,b6,b7;
		b1=(a>>i)&1;
		b4=(a>>((i+4)%8))&1;
		b5=(a>>((i+5)%8))&1;
		b6=(a>>((i+6)%8))&1;
		b7=(a>>((i+7)%8))&1;
		b2=(c>>i)&1;
		b+=((b1+b4+b5+b6+b7+b2)%2)<<i;
	}
	return b;
}

void addRoundKey()
{
	int i,j,k,l,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=state[i][j]^Key[i][j];
	}
	int temp[4][4],tmp[4];
	for(j=3;j>=0;j--)
	{
		if(j==3)
		{
			for(k=0;k<4;k++)
				tmp[k]=Key[(k+1)%4][j];
			for(k=0;k<4;k++)
			{
					l=tmp[k];
					i1=(l>>4);
					k1=(i1<<4);
					j1=l^k1;
					temp[k][j]=sbox[i1][j1];
					if(k==0)
						temp[k][j]=temp[k][j]^1;
			}
			for (i=1;i<4;i++)
				temp[k][0]=temp[k][j]^Key[k][0];			
		}
		else
		{
			for (i=1;i<4;i++)
				temp[i][3-j]=temp[i][2-j]^Key[i][3-j];
		}
	}
}

void subBytes()
{
	int i,j,k,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			k=state[i][j];
			i1=(k>>4);
			k1=(i1<<4);
			j1=k^k1;
			state[i][j]=sbox[i1][j1];
		}
	}
}

void shiftRow()
{
	int i,j,temp[4];
	for (i=0;i<4;i++)
	{
		if (i>0)
		{
			for (j=0;j<4;j++)
				temp[j]=state[i][(j+i)%4];
			for (j=0;j<4;j++)
				state[i][j]=temp[j];	
		}
	}
}

void mixColumns()
{
	int i,j,k,new,t,temp[4][4];
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			temp[i][j]=0;
			for(k=0;k<4;k++)
			{
				if(mulCol[j][k]==0x1)
					temp[i][j]^=state[k][i];
				else if(mulCol[j][k]==0x2)
				{
					new=state[k][i]<<1;
					t=new>>8;
					if (t>0)
						new=new^(t<<8)^1;
					temp[i][j]^=new;
				}
				else
				{
					new=state[k][i]<<1;
					t=new>>8;
					if (t>0)
						new=new^(t<<8)^1;
					new=new^state[i][j];
					temp[i][j]^=new;
				}
			}
		}
	}
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=temp[i][j];
	}
}

void sBox()
{
	char i,j;
	int k,c=0x63;
	for (i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			k=i;
			k=(k<<4);
			k=k+j;
			sbox[i][j]=inv(k);
		}
	}
}

int main()
{
	key="Thats my Kung Fu";
	str="Two One Nine Two";
	printf("Input string (PlainText) of 16 length string: %s\n",str);
	printf("Key of 16 length string: %s\n",key);
	int i,j,k;
	sBox();
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(j==i)
				mulCol[i][j]=0x2;
			else if((j-1)%4==1)
				mulCol[i][j]=0x3;
			else
				mulCol[i][j]=0x1;
		}
	}
	k=0;
	for(j=0;j<4;j++)
	{
		for (i=0;i<4;i++)
		{
			Key[i][j]=key[k];
			state[i][j]=str[k];
			k++;
		}
	}
	addRoundKey();
	for(i=1;i<11;i++)
	{
		subBytes();
		shiftRow();
		mixColumns();
		if (i<10)
			addRoundKey();	
		i++;
	}
	printf("The ciphertext is: ");
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			printf(" 0x%0.2x ",state[j][i]);
	}
	return 0;
}
