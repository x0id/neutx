
bin_PROGRAMS = test_neutx

test_neutx_SOURCES = \
	test_atomic_value.cpp \
	test_ptrie.cpp test_actrie.cpp \
	test_timeconv.cpp \
	test_main.cpp

test_neutx_CPPFLAGS = \
	-I../include \
	-DBOOST_TEST_DYN_LINK \
	$(BOOST_CPPFLAGS)

test_neutx_LDADD = \
	$(BOOST_LDFLAGS) \
	$(BOOST_CHRONO_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIB)
