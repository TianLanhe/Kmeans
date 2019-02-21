#include "Log.h"
#include "Common.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace KMeans;

Log createGlobalLogger(){
	Log log;

	log.add(&cout);

	ofstream* out = new ofstream(LOG_FILE);
	if(out){
		log.add(out);
	}

	return log;
}

Log createGlobalOutputer() {
	Log log;

	log.add(&cout);

	ofstream* out = new ofstream(RESULT_FILE);
	if (out) {
		log.add(out);
	}

	return log;
}

Log KMeans::log = createGlobalLogger();
Log KMeans::out = createGlobalOutputer();
