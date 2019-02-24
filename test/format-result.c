int a = 0, b = 5; 
void foo(char* x, int y)
{
    *(int*)x += y; 
    for(int i = 0; i < y; i++)
    {
        if (!(i <= 0.5))
        {
            switch(i)
            {
                case 0 : 
                    ++i; 
                    break; 
                case 100 : 
                    i += 2; 
                    break; 
                default : 
                    i *= 2; 
                    break; 
            }
        }
        else if(i > 1e16)
            continue; 
        else
            if (i)
                return; 
    }
    do
    {
        while(1)
            loop(&x); 
    }while(0); 
    return; 
}
int main()
{
    printf("Hello World!\n"); 
}
