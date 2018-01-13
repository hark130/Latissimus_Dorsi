int main(void)
{
	__asm__("mov $0x400430, %rax");
	__asm__("jmp *%rax");
}