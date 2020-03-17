#
# Regular cron jobs for the fort-studio package
#
0 4	* * *	root	[ -x /usr/bin/fort-studio_maintenance ] && /usr/bin/fort-studio_maintenance
