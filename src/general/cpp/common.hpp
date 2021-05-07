#ifndef COMMON_CPP
#define COMMON_CPP

template <typename T>
const T &min(const T &first, const T &second) {
	return first < second ? first : second;
}

template <typename T>
const T &max(const T &first, const T &second) {
	return second < first ? first : second;
}

#endif // COMMON_CPP