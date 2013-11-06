static void _dummy(int c)
{
}

void (*_putchar)(int c) = _dummy;

int putchar(int c)
{
    _putchar(c);
    return c;
}

