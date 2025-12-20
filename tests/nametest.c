/*
 * Boundary tests for filename handling.
 */

#include	<stdio.h>
#include	<string.h>

#include	<vbisam.h>

static int
expect_efname (int rc, const char *label)
{
	if (rc != -1) {
		printf ("%s: expected failure, got %d\n", label, rc);
		return 1;
	}
	if (iserrno != EFNAME) {
		printf ("%s: expected EFNAME, got %d\n", label, iserrno);
		return 1;
	}
	return 0;
}

int
main (void)
{
	char	longname[1100];
	struct keydesc skey;
	int		failures = 0;

	memset (longname, 'a', sizeof (longname));
	longname[1020] = '\0';

	memset (&skey, 0, sizeof (skey));
	skey.k_nparts = 1;
	skey.k_start = 0;
	skey.k_leng = 1;
	skey.k_type = CHARTYPE;

	iserrno = 0;
	failures += expect_efname (iserase (longname), "iserase");

	iserrno = 0;
	failures += expect_efname (isrename (longname, "b"), "isrename");

	iserrno = 0;
	failures += expect_efname (isopen (longname, ISINOUT), "isopen");

	iserrno = 0;
	failures += expect_efname (isbuild (longname, 10, &skey, ISINOUT), "isbuild");

	if (failures) {
		printf ("nametest: %d failure(s)\n", failures);
		return 1;
	}
	printf ("nametest: ok\n");
	return 0;
}
