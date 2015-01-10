#include <iostream>
#include <string>
#include <raptor2.h>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

const string filename(
		"/home/szarnyasg/git/trainbenchmark/trainbenchmark-models/railway-xform-2048.ttl");
const string RDF_TYPE("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
const string BASE_PREFIX(
		"http://www.semanticweb.org/ontologies/2011/1/TrainRequirementOntology.owl#");
set<long> switches;
multimap<long, long> trackElement_sensors;

long get_id(string s) {
	regex pattern(BASE_PREFIX + "x(\\d+)");
	smatch result;
	if (regex_search(s, result, pattern)) {
		string submatch(result[1].first, result[1].second);
		long x = atol(submatch.c_str());
		return x;
	}

	return 0;
}

string get_type(string s) {
	static regex pattern(BASE_PREFIX + "(.*)");
	smatch result;
	if (regex_search(s, result, pattern)) {
		string submatch(result[1].first, result[1].second);
		return submatch;
	}
	return "";
}

void print_triple(void* user_data, raptor_statement* triple) {
	raptor_uri* subject_uri = triple->subject->value.uri;
	string subject(
			reinterpret_cast<const char*>(raptor_uri_as_string(subject_uri)));

	raptor_uri* predicate_uri = triple->predicate->value.uri;
	string predicate(
			reinterpret_cast<const char*>(raptor_uri_as_string(predicate_uri)));

	if (!predicate.compare(RDF_TYPE)) {
		raptor_uri* object_uri = triple->object->value.uri;
		string object(
				reinterpret_cast<const char*>(raptor_uri_as_string(object_uri)));
		string type = get_type(object);

		if (!type.compare("Switch")) {
			switches.insert(get_id(subject));
		}
	} else if (!predicate.compare(BASE_PREFIX + "TrackElement_sensor")) {
		raptor_uri* object_uri = triple->object->value.uri;
		string object(
				reinterpret_cast<const char*>(raptor_uri_as_string(object_uri)));

		trackElement_sensors.insert(
				multimap<long, long>::value_type(get_id(subject),
						get_id(object)));
	}
}

void process() {
	set<long> trackElements;
	for (multimap<long, long>::iterator itr = trackElement_sensors.begin();
			itr != trackElement_sensors.end();) {
		multimap<long, long>::iterator curr = itr;
		while (itr != trackElement_sensors.end() && itr->first == curr->first) {
			trackElements.insert(itr->first);
			++itr;
		}
	}

	long n = 0;
	for (set<long>::iterator it = switches.begin(); it != switches.end(); it++) {
		if (trackElements.find(*it) == trackElements.end()) {
			n++;
		}
	}
	cout << "Results: " << n << endl;

}

void print_inputs() {
	cout << "switches: " << endl;
	for (set<long>::iterator it = switches.begin(); it != switches.end(); it++) {
		cout << *it << ", ";
	}

	cout << endl;
	cout << "trackElement_sensors: " << endl;
	for (multimap<long, long>::iterator itr = trackElement_sensors.begin();
			itr != trackElement_sensors.end();) {
		multimap<long, long>::iterator curr = itr;
		while (itr != trackElement_sensors.end() && itr->first == curr->first) {
			cout << itr->first << "-" << itr->second << ", ";
			++itr;
		}
	}
}

int main() {
	clock_t begin_load = clock();

	raptor_world *world = NULL;
	raptor_parser* rdf_parser = NULL;
	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	world = raptor_new_world();

	rdf_parser = raptor_new_parser(world, "turtle");

	raptor_parser_set_statement_handler(rdf_parser, NULL, print_triple);

	uri_string = raptor_uri_filename_to_uri_string(filename.c_str());
	uri = raptor_new_uri(world, uri_string);
	base_uri = raptor_uri_copy(uri);

	raptor_parser_parse_file(rdf_parser, uri, base_uri);

	raptor_free_parser(rdf_parser);

	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);

	raptor_free_world(world);
	clock_t end_load = clock();
	double elapsed_secs_load = double(end_load - begin_load) / CLOCKS_PER_SEC;
	cout << elapsed_secs_load << endl;

	clock_t begin_proc = clock();
	process();
	clock_t end_proc = clock();
	double elapsed_secs_proc = double(end_proc - begin_proc) / CLOCKS_PER_SEC;

	cout << elapsed_secs_proc << endl;

	//print_inputs();

	return 0;
}
