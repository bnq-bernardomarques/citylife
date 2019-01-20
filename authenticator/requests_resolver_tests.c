#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "test.h"
#include "requests_resolver.h"

char *callback_1(auth_request *r) {
  return "first";
}

char *callback_2(auth_request *r) {
  return "second";
}

char *callback_aggregate(auth_request *r) {
  char *agg = (char *)calloc(64, sizeof(char));
  
  if (r->username != NULL){
    sprintf(agg + strlen(agg), "%s", r->username);
  }
    
  if (r->password != NULL) {
    sprintf(agg + strlen(agg), "%s", r->password);
  }

  if (r->session_token != NULL) {
    sprintf(agg + strlen(agg), "%s", r->session_token);
  }
  
  return agg;
}

void setup_env() {
  requests_set_callback(AUTH_NEW, callback_aggregate);
  requests_set_callback(AUTH_AUTH, callback_1);
  requests_set_callback(AUTH_DELETE, callback_2);
}

char before_test() {
  return 0;
}

char after_test() {
  return 0;
}

void clean_env() {
}

char test_requests_resolve_parse_well_formed_request() {

  char *auth_request = "{"
    "\"action\": \"new\","
    "\"user\": \"ze\","
    "\"pass\": \"qwerty\""
  "}";
  
  char *response = requests_resolve(auth_request);
  if (assert_str_equals("The request fields were correctly parsed", response, "zeqwerty")) {
    free(response);
    return 1;
  }

  free(response);
  return 0;
}

char test_requests_resolve_ensure_correct_callbacks_are_called() {
  char *auth_request = "{"
    "\"action\": \"auth\","
    "\"user\": \"ze\","
    "\"pass\": \"qwerty\""
  "}";
  
  char *response = requests_resolve(auth_request);
  if (assert_str_equals("The correct callback was called.", response, "first")) {
    return 1;
  }
  
  char *validate_request = "{"
    "\"action\": \"delete\","
    "\"sess\": \"xdg\""
  "}";
  
  response = requests_resolve(validate_request);
  if (assert_str_equals("The correct callback was called", response, "second")) {
    return 1;
  }

  return 0;
}

test test_suite[] = {
  {
    "Test parsing requests with different actions", test_requests_resolve_ensure_correct_callbacks_are_called
  },
  {
    "Test parsing a well formed request", test_requests_resolve_parse_well_formed_request
  },
};

int main(int argc, char **argv) {
  suite_report report = run_test_suite(test_suite, sizeof(test_suite)/sizeof(test));

  if (report.failures > 0) {
    return -1;
  }

  return 0;
}
