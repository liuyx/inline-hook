It's a simple inline-hook framework which its usage is very simple:

if (hook(<hooked_address>,<new_address>) < 0) {
	fprintf(stderr,"It got error!");
	exit(1);
}

unhook is also a peace of cake:

if (unhook(<hooked_address>) < 0) {
	fprintf(stderr,"it got a error\n");
	exit(1);
}
