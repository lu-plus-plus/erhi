#pragma once

#include <string>				// for error message
#include <source_location>		// for where exception is thrown
#include <type_traits>			// for is_enum



namespace erhi {

	struct base_exception {
		base_exception();
		virtual ~base_exception();
		
		virtual char const * what() const;
	};

	struct bad_api_call : base_exception {
		std::string mMessage;

		bad_api_call(char const * result, char const * statement, std::source_location const & location);
		virtual ~bad_api_call() override;

		virtual char const * what() const override;
	};

	struct invalid_arguments : base_exception {
		std::string mMessage;

		invalid_arguments(char const * message);
		invalid_arguments(std::string const & message);
		invalid_arguments(std::string && message);
		virtual ~invalid_arguments() override;

		virtual char const * what() const override;
	};

	//namespace details {

	//	template <size_t N>
	//	struct StringLiteral {
	//		constexpr StringLiteral(char const * stringLiteral) {
	//			for (size_t i = 0; i < N; ++i) {
	//				mString[i] = stringLiteral[i];
	//			}
	//		}
	//		char mString[N];
	//	};

	//	template <typename Enum>
	//	struct EnumReflection {

	//		static constexpr int FindFirst(char const * stringLiteral, char target) {
	//			for (int i = 0; stringLiteral[i] != '\0'; ++i) {
	//				if (stringLiteral[i] == target) {
	//					return i;
	//				}
	//			}
	//			return -1;
	//		}

	//		static constexpr int GetEnumNameLength() {
	//			char const * prettyFunction = __FUNCSIG__;
	//			int nameBegin = FindFirst(prettyFunction, '<') + 1;
	//			int nameEnd = FindFirst(prettyFunction, '>');
	//			return nameEnd - nameBegin;
	//		}

	//		char mEnumName[GetEnumNameLength() + 1];

	//		constexpr EnumReflection(Enum value) : mEnumName() {
	//			char const * prettyFunction = __FUNCSIG__;
	//			int nameBegin = FindFirst(prettyFunction, '<') + 1;
	//			std::copy_n(prettyFunction + nameBegin, GetEnumNameLength(), mEnumName);
	//			mEnumName[GetEnumNameLength()] = '\0';
	//		}

	//	};

	//}

	//template <typename Enum>
	//struct bad_enum_class : invalid_arguments {
	//	bad_enum_class(Enum value);
	//	virtual ~bad_enum_class() override;

	//	virtual char const * what() const override;
	//};

}