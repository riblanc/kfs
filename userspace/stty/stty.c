/* stty: show and change the settings of the terminal on standard input.
 *
 * Only what the ShadokOS line discipline understands, plus the flags it
 * stores without acting on them. A setting is a flag name, the same name
 * prefixed with '-' to turn it off, one of the combinations below, or a
 * control character given as `name value`. */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

enum word { IFLAG, OFLAG, CFLAG, LFLAG };

struct flag {
	const char *name;
	tcflag_t bit;
	enum word word;
};

/* Flags the kernel acts on come first in each group; the rest are stored and
 * ignored, and are still worth showing. */
static const struct flag flags[] = {
	{ "istrip", ISTRIP, IFLAG }, { "icrnl", ICRNL, IFLAG },
	{ "inlcr", INLCR, IFLAG },   { "igncr", IGNCR, IFLAG },
	{ "brkint", BRKINT, IFLAG }, { "ignbrk", IGNBRK, IFLAG },
	{ "ignpar", IGNPAR, IFLAG }, { "inpck", INPCK, IFLAG },
	{ "parmrk", PARMRK, IFLAG }, { "ixon", IXON, IFLAG },
	{ "ixoff", IXOFF, IFLAG },   { "ixany", IXANY, IFLAG },

	{ "opost", OPOST, OFLAG },   { "onlcr", ONLCR, OFLAG },
	{ "ocrnl", OCRNL, OFLAG },   { "onlret", ONLRET, OFLAG },
	{ "olcuc", OLCUC, OFLAG },   { "onocr", ONOCR, OFLAG },
	{ "ofill", OFILL, OFLAG },   { "ofdel", OFDEL, OFLAG },

	{ "cstopb", CSTOPB, CFLAG }, { "cread", CREAD, CFLAG },
	{ "parenb", PARENB, CFLAG }, { "parodd", PARODD, CFLAG },
	{ "hupcl", HUPCL, CFLAG },   { "clocal", CLOCAL, CFLAG },

	{ "icanon", ICANON, LFLAG }, { "echo", ECHO, LFLAG },
	{ "echoe", ECHOE, LFLAG },   { "echok", ECHOK, LFLAG },
	{ "echonl", ECHONL, LFLAG }, { "echoctl", ECHOCTL, LFLAG },
	{ "isig", ISIG, LFLAG },     { "noflsh", NOFLSH, LFLAG },
	{ "iexten", IEXTEN, LFLAG }, { "tostop", TOSTOP, LFLAG },
};

static const struct cchar {
	const char *name;
	int index;
} cchars[] = {
	{ "intr", VINTR },   { "quit", VQUIT }, { "erase", VERASE },
	{ "kill", VKILL },   { "eof", VEOF },   { "eol", VEOL },
	{ "start", VSTART }, { "stop", VSTOP }, { "susp", VSUSP },
};

static const tcflag_t *word_of_const(const struct termios *t, enum word w)
{
	switch (w) {
	case IFLAG: return &t->c_iflag;
	case OFLAG: return &t->c_oflag;
	case CFLAG: return &t->c_cflag;
	default:    return &t->c_lflag;
	}
}

static tcflag_t *word_of(struct termios *t, enum word w)
{
	return (tcflag_t *)word_of_const(t, w);
}

/* ^C, ^?, a bare character, or "undef" for a character that is disabled. */
static int parse_cc(const char *s, cc_t *out)
{
	if (!strcmp(s, "undef") || !strcmp(s, "^-")) {
		*out = 0;
		return 0;
	}
	if (s[0] == '^' && s[1] && !s[2]) {
		*out = (s[1] == '?') ? 127 : (cc_t)(s[1] & 0x1f);
		return 0;
	}
	if (s[0] && !s[1]) {
		*out = (cc_t)s[0];
		return 0;
	}
	return -1;
}

static void print_cc(const char *name, cc_t c)
{
	if (c == 0)
		printf("%s = <undef>; ", name);
	else if (c == 127)
		printf("%s = ^?; ", name);
	else if (c < 32)
		printf("%s = ^%c; ", name, c + '@');
	else
		printf("%s = %c; ", name, c);
}

static void show(const struct termios *t)
{
	for (size_t i = 0; i < sizeof(cchars) / sizeof(*cchars); i++)
		print_cc(cchars[i].name, t->c_cc[cchars[i].index]);
	printf("min = %d; time = %d;\n", t->c_cc[VMIN], t->c_cc[VTIME]);

	enum word last = IFLAG;
	for (size_t i = 0; i < sizeof(flags) / sizeof(*flags); i++) {
		const struct flag *f = &flags[i];
		if (i && f->word != last)
			printf("\n");
		last = f->word;
		printf("%s%s ", (*word_of_const(t, f->word) & f->bit) ? "" : "-", f->name);
	}
	printf("\n");
}

/* raw and cooked are the two the kernel really distinguishes; sane is what a
 * terminal starts as. */
static int combination(struct termios *t, const char *name)
{
	if (!strcmp(name, "raw") || !strcmp(name, "-cooked")) {
		t->c_iflag &= ~(ICRNL | INLCR | IGNCR | ISTRIP | IXON);
		t->c_oflag &= ~OPOST;
		t->c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ISIG | IEXTEN);
		t->c_cc[VMIN] = 1;
		t->c_cc[VTIME] = 0;
		return 1;
	}
	if (!strcmp(name, "cooked") || !strcmp(name, "-raw") || !strcmp(name, "sane")) {
		t->c_iflag |= ICRNL | IXON | BRKINT;
		t->c_iflag &= ~(INLCR | IGNCR | ISTRIP);
		t->c_oflag |= OPOST | ONLCR;
		t->c_lflag |= ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ISIG | IEXTEN;
		t->c_lflag &= ~(ECHONL | NOFLSH | TOSTOP);
		if (strcmp(name, "sane"))
			return 1;
		t->c_cc[VINTR] = 3;    /* ^C */
		t->c_cc[VQUIT] = 28;   /* ^\ */
		t->c_cc[VERASE] = 8;   /* ^H */
		t->c_cc[VKILL] = 21;   /* ^U */
		t->c_cc[VEOF] = 4;     /* ^D */
		t->c_cc[VEOL] = 0;
		t->c_cc[VSTART] = 17;  /* ^Q */
		t->c_cc[VSTOP] = 19;   /* ^S */
		t->c_cc[VSUSP] = 26;   /* ^Z */
		t->c_cc[VMIN] = 0;
		t->c_cc[VTIME] = 0;
		return 1;
	}
	return 0;
}

/* Returns how many arguments were used, or 0 when the setting is unknown. */
static int setting(struct termios *t, char **av)
{
	const char *name = av[0];
	int on = 1;

	if (combination(t, name))
		return 1;

	if (!strcmp(name, "min") || !strcmp(name, "time")) {
		if (!av[1]) {
			fprintf(stderr, "stty: %s takes a number\n", name);
			exit(1);
		}
		t->c_cc[!strcmp(name, "min") ? VMIN : VTIME] = (cc_t)atoi(av[1]);
		return 2;
	}

	for (size_t i = 0; i < sizeof(cchars) / sizeof(*cchars); i++) {
		if (strcmp(name, cchars[i].name))
			continue;
		if (!av[1] || parse_cc(av[1], &t->c_cc[cchars[i].index]) < 0) {
			fprintf(stderr, "stty: %s takes a character, ^X or undef\n", name);
			exit(1);
		}
		return 2;
	}

	if (name[0] == '-') {
		on = 0;
		name++;
	}
	for (size_t i = 0; i < sizeof(flags) / sizeof(*flags); i++) {
		if (strcmp(name, flags[i].name))
			continue;
		tcflag_t *word = word_of(t, flags[i].word);
		if (on)
			*word |= flags[i].bit;
		else
			*word &= ~flags[i].bit;
		return 1;
	}
	return 0;
}

int main(int ac, char **av)
{
	struct termios t;

	if (tcgetattr(0, &t) < 0) {
		perror("stty: standard input");
		return 1;
	}

	if (ac == 1 || (ac == 2 && !strcmp(av[1], "-a"))) {
		show(&t);
		return 0;
	}

	for (int i = 1; i < ac;) {
		int used = setting(&t, av + i);
		if (!used) {
			fprintf(stderr, "stty: unknown setting: %s\n", av[i]);
			return 1;
		}
		i += used;
	}

	if (tcsetattr(0, TCSAFLUSH, &t) < 0) {
		perror("stty");
		return 1;
	}
	return 0;
}
