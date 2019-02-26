#include "Log.h"
#include "Common.h"
#include <fstream>
#include <iostream>

using namespace std;

Log createGlobalOutputer() {
	Log log;

	log.add(&cout);

	ofstream* out = new ofstream(RESULT_FILE);
	if (out) {
		log.add(out);
	}

	return log;
}

Log out = createGlobalOutputer();
