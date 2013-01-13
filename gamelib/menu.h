
#define MENU_MAXSIZE 10;



struct menu {
	int selected;
	int length;
	struct menu_entry entries*;
	double animation;
};




struct menu_entry {
	char* (* gettext)();
	void (* upaction)();
	void (* downaction)();
	void (* caction)();
	void (* zaction)();	
};
