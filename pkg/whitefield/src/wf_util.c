#include "whitefield.h"

#include "net/ieee802154.h"

extern uint16_t wf_nodeid;	//originally defined in cpu/native/startup.c
uint16_t wf_get_nodeid(void)
{
	return wf_nodeid;
}

#if 0
int usage(char *cmd)
{
	printf("Usage: %s -n <nodeid>\n\n", cmd);
	return 0;
}

int wf_parse_cmdline(int argc, char **argv)
{
	wf_node_info_t *node=&g_node_info;
	int c;

	opterr=0;
	node->id=0xffff;
	LOG("parsing\n");
	while((c=getopt(argc, argv, "n:")) != -1) {
		switch(c) {
			case 'n':
				node->id = atoi(optarg);
				break;
			case '?':
				if(isprint(optopt)) {
					ERR("Option -%c requires an arg\n", optopt);
				} else {
					ERR("Unknown option char '\\x%x'.\n", optopt);
				}
				goto FAILRET;
			default:
				goto FAILRET;
		}
	}
	if(0xffff == node->id) {
		ERR("Need node id info mandatorily\n");
		goto FAILRET;
	}
	LOG("Node id <dec=%d> <hex=%x>\n", node->id, node->id);
	return 0;
FAILRET:
	usage(argv[0]);
	abort();
	return -1;
}
#endif
