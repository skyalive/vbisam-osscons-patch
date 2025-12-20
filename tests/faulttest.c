/*
 * Fault injection tests for error propagation.
 */

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	<vbisam.h>

static int
expect_errno (int rc, int expected, const char *label)
{
	if (rc != -1) {
		printf ("%s: expected failure, got %d\n", label, rc);
		return 1;
	}
	if (iserrno != expected) {
		printf ("%s: expected %d, got %d\n", label, expected, iserrno);
		return 1;
	}
	return 0;
}

static int
touch_file (const char *path)
{
	int fd = open (path, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd < 0) {
		return -1;
	}
	close (fd);
	return 0;
}

int
main (void)
{
	const char	*fname = "FaultTest";
	const char	*logname = "FAULTLOG";
	struct keydesc	skey;
	char		row[10];
	int		handle;
	int		failures = 0;

	memset (&skey, 0, sizeof (skey));
	skey.k_nparts = 1;
	skey.k_start = 0;
	skey.k_leng = 1;
	skey.k_type = CHARTYPE;

	memset (row, 'A', sizeof (row));

	iserase (fname);
	handle = isbuild (fname, (int)sizeof (row), &skey, ISINOUT + ISFIXLEN);
	if (handle < 0) {
		printf ("faulttest: build failed %d\n", iserrno);
		return 1;
	}

	if (iswrite (handle, row)) {
		printf ("faulttest: write failed %d\n", iserrno);
		return 1;
	}

	if (isclose (handle)) {
		printf ("faulttest: close failed %d\n", iserrno);
		return 1;
	}

	handle = isopen (fname, ISINOUT + ISFIXLEN + ISEXCLLOCK);
	if (handle < 0) {
		printf ("faulttest: reopen excl failed %d\n", iserrno);
		return 1;
	}

	setenv ("VBISAM_FAULT_FORCEEXIT", "1", 1);
	failures += expect_errno (isclose (handle), EBADFILE, "forceexit");
	unsetenv ("VBISAM_FAULT_FORCEEXIT");

	if (isclose (handle)) {
		printf ("faulttest: close2 failed %d\n", iserrno);
		return 1;
	}

	if (touch_file (logname) != 0) {
		printf ("faulttest: log create failed\n");
		return 1;
	}
	if (islogopen (logname)) {
		printf ("faulttest: log open failed %d\n", iserrno);
		return 1;
	}

	handle = isopen (fname, ISINOUT + ISFIXLEN);
	if (handle < 0) {
		printf ("faulttest: reopen failed %d\n", iserrno);
		return 1;
	}

	memset (row, 'B', sizeof (row));
	if (iswrite (handle, row)) {
		printf ("faulttest: write2 failed %d\n", iserrno);
		return 1;
	}

	setenv ("VBISAM_FAULT_TRANSDELETE", "1", 1);
	failures += expect_errno (isdelete (handle, row), ELOGWRIT, "transdelete");
	unsetenv ("VBISAM_FAULT_TRANSDELETE");

	islogclose ();
	isclose (handle);
	iserase (fname);
	unlink (logname);

	if (failures) {
		printf ("faulttest: %d failure(s)\n", failures);
		return 1;
	}
	printf ("faulttest: ok\n");
	return 0;
}
