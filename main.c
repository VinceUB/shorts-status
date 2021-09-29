#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#define PLATFORM "linux"

int main(){
	char* output_string;
	size_t output_string_size;
	CURL* curl;
	curl_global_init(
			CURL_GLOBAL_SSL |
			CURL_GLOBAL_WIN32
			);
	
	curl = curl_easy_init();

	if(!curl){
		fprintf(stderr, "Something went wrong in curl");
	}

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.reddit.com/r/IsEricWearingShorts/new.json?limit=1");
	curl_easy_setopt(curl, CURLOPT_USERAGENT, PLATFORM ":net.ultrabanana.eric:v0.0.1 (by /u/vkb123)");

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);

	FILE* output_fd = open_memstream(&output_string, &output_string_size);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_fd);

	curl_easy_perform(curl);
	free(curl);
	fflush(output_fd);


	cJSON* json = cJSON_Parse(output_string);
	free(output_string);

	cJSON* data1 = cJSON_GetObjectItem(json, "data");
	cJSON* children = cJSON_GetObjectItem(data1, "children");
	cJSON* firstThingFromChildrenFuckYouJason = cJSON_GetArrayItem(children, 0);
	cJSON* data2 = cJSON_GetObjectItem(firstThingFromChildrenFuckYouJason, "data");
	cJSON* answer_json = cJSON_GetObjectItem(data2, "selftext");
	cJSON* time_json = cJSON_GetObjectItem(data2, "created_utc");

	free(data1);free(children);free(firstThingFromChildrenFuckYouJason);free(data2);
	
#define TIME_BUFFER 20
	char* time_string = calloc(sizeof(char), TIME_BUFFER);
	time_t time_time = (long long int) time_json->valuedouble;
	struct tm* time_struct = localtime(&time_time); //TODO: Convert to locale-preferred time
	
	printf("Is Eric wearing shorts (as of %d-%d-%d)?\n%s\n", 
			time_struct->tm_year+1900, time_struct->tm_mon, time_struct->tm_mday,
			answer_json->valuestring); //TODO: Make it prettier. Needs some kind of logic
}
