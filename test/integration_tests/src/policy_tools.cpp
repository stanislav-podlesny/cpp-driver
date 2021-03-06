/*
  Copyright (c) 2014-2015 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <boost/test/test_tools.hpp>
#include <boost/test/debug.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>

#include "testing.hpp"
#include "policy_tools.hpp"

void PolicyTool::show_coordinators()	// show what queries went to what nodes IP.
{
  for (std::map<std::string, int>::const_iterator p = coordinators.begin(); p != coordinators.end(); ++p) {
    std::cout << p->first << " : " << p->second << std::endl;
  }
}

void PolicyTool::reset_coordinators()
{
  coordinators.clear();
}

<<<<<<< HEAD
		
void show_coordinators()				// show what queries went to what nodes IP.	
{
	for( std::map<boost::asio::ip::address, int>::const_iterator p = coordinators.begin(); p != coordinators.end(); ++p )
	{
		std::cout << p->first.to_string() << " : " << p->second << std::endl;
	}
}

void 
create_schema(
	boost::shared_ptr<cql::cql_session_t> session,
	int replicationFactor)
{		
	test_utils::query(session, str(boost::format(test_utils::CREATE_KEYSPACE_SIMPLE_FORMAT) % test_utils::SIMPLE_KEYSPACE % replicationFactor));
	session->set_keyspace(test_utils::SIMPLE_KEYSPACE);
	test_utils::query(session, str(boost::format("CREATE TABLE %s (k int PRIMARY KEY, i int)") % test_utils::SIMPLE_TABLE));
=======
void PolicyTool::create_schema(CassSession* session, int replicationFactor)
{
  test_utils::execute_query(session,
                            str(boost::format(test_utils::CREATE_KEYSPACE_SIMPLE_FORMAT) % test_utils::SIMPLE_KEYSPACE % replicationFactor));
  test_utils::execute_query(session, str(boost::format("USE %s") % test_utils::SIMPLE_KEYSPACE));
  test_utils::execute_query(session,
                            str(boost::format("CREATE TABLE %s (k int PRIMARY KEY, i int)") % test_utils::SIMPLE_TABLE));
}

void PolicyTool::create_schema(CassSession* session, int dc1, int dc2)
{
  test_utils::execute_query(session,
                            str(boost::format(test_utils::CREATE_KEYSPACE_NETWORK_FORMAT) % test_utils::SIMPLE_KEYSPACE % dc1 % dc2));
  test_utils::execute_query(session, str(boost::format("USE %s") % test_utils::SIMPLE_KEYSPACE));
  test_utils::execute_query(session,
                            str(boost::format("CREATE TABLE %s (k int PRIMARY KEY, i int)") % test_utils::SIMPLE_TABLE));
>>>>>>> upstream/master
}

void PolicyTool::init(CassSession* session, int n, CassConsistency cl, bool batch)
{
  std::string query = str(boost::format("INSERT INTO %s (k, i) VALUES (0, 0)") % test_utils::SIMPLE_TABLE);

  if (batch) {
    std::string bth;
    bth.append("BEGIN BATCH ");
    bth.append(str(boost::format("INSERT INTO %s (k, i) VALUES (0, 0)") % test_utils::SIMPLE_TABLE));
    bth.append(" APPLY BATCH");
    query = bth;
  }

  for (int i = 0; i < n; ++i) {
    test_utils::CassResultPtr result;
    test_utils::execute_query(session,
                              query,
                              &result,
                              cl);
  }
}

CassError PolicyTool::init_return_error(CassSession* session, int n, CassConsistency cl, bool batch)
{
  std::string query = str(boost::format("INSERT INTO %s (k, i) VALUES (0, 0)") % test_utils::SIMPLE_TABLE);

  if (batch) {
    std::string batch_query;
    batch_query.append("BEGIN BATCH ");
    batch_query.append(str(boost::format("INSERT INTO %s (k, i) VALUES (0, 0)") % test_utils::SIMPLE_TABLE));
    batch_query.append(" APPLY BATCH");
    query = batch_query;
  }

  for (int i = 0; i < n; ++i) {
    test_utils::CassResultPtr result;
    CassError rc = test_utils::execute_query_with_error(session,
                                                        query,
                                                        &result,
                                                        cl);
    if (rc != CASS_OK) {
      return rc;
    }
  }
  return CASS_OK;
}

void PolicyTool::add_coordinator(std::string address)
{
  if (coordinators.count(address) == 0) {
    coordinators.insert(std::make_pair(address, 1));
  } else {
    coordinators[address] += 1;
  }
}

void PolicyTool::assert_queried(std::string address, int n)
{
  if (coordinators.count(address) != 0) {
    int c = coordinators[address];
    BOOST_REQUIRE(c == n);
  } else {
    BOOST_REQUIRE(n == 0);
  }
}

void PolicyTool::assertQueriedAtLeast(std::string address, int n)
{
  int queried = coordinators[address];
  BOOST_REQUIRE(queried >= n);
}

void PolicyTool::query(CassSession* session, int n, CassConsistency cl)
{
<<<<<<< HEAD
	for (int i = 0; i < n; ++i)
	{
		boost::shared_ptr<cql::cql_query_t> _query(
			new cql::cql_query_t(str(boost::format("SELECT * FROM %s WHERE k = 0") % test_utils::SIMPLE_TABLE),cl));
        
        boost::shared_future<cql::cql_future_result_t> query_future;

        query_future = session->query(_query);
        if (!(query_future.timed_wait(boost::posix_time::seconds(20)))) {
            BOOST_FAIL("Query timed out");
        }
            
        try {
            cql::cql_future_result_t query_result = query_future.get();
        
            if (query_future.has_value()) {
                std::cout << "Queried endpoint: " << query_result.client->endpoint().to_string() << std::endl;
                add_coordinator(query_result.client->endpoint().address());
                if(query_result.error.code != 0) {
                    return query_result.error.code;
                }
            }
        }
        catch (...) {
            BOOST_MESSAGE("Response contains EXCEPTION");
            throw;
        }
	}
	return 0;
=======
  std::string select_query = str(boost::format("SELECT * FROM %s WHERE k = 0") % test_utils::SIMPLE_TABLE);
  for (int i = 0; i < n; ++i) {
    test_utils::CassStatementPtr statement(cass_statement_new_n(select_query.data(), select_query.size(), 0));
    cass_statement_set_consistency(statement.get(), cl);
    test_utils::CassFuturePtr future(cass_session_execute(session, statement.get()));
    test_utils::wait_and_check_error(future.get());
    add_coordinator(cass::get_host_from_future(future.get()));
  }
>>>>>>> upstream/master
}

CassError PolicyTool::query_return_error(CassSession* session, int n, CassConsistency cl)
{
  std::string select_query = str(boost::format("SELECT * FROM %s WHERE k = 0") % test_utils::SIMPLE_TABLE);
  for (int i = 0; i < n; ++i) {
    test_utils::CassStatementPtr statement
        (cass_statement_new_n(select_query.data(), select_query.size(), 0));
    cass_statement_set_consistency(statement.get(), cl);
    test_utils::CassFuturePtr future(cass_session_execute(session, statement.get()));
    CassError rc = test_utils::wait_and_return_error(future.get());

    if (rc != CASS_OK) {
      return rc;
    }
    add_coordinator(cass::get_host_from_future(future.get()));
  }
  return CASS_OK;
}
