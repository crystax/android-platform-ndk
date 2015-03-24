__thread int __v;

void tga_set_value(int v)
{
    __v = v;
}

int tga_value()
{
    return __v;
}
