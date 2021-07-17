
fmt:
	cmake-format -i *.cmake
	clang-format -i *.cpp

bench: clean
	( mkdir -p build; cd build ; cmake $(CMAKE_PROFILE_OPTIONS) .. ; $(MAKE) VERBOSE=1 -j )
	$(SHOW) build/simd_utils_bench --benchmark_min_time=1

clean: distclean

distclean:
	rm -rf build/* 