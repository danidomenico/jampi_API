#ifndef _RANGE_
#define _RANGE_

namespace parallel {

class Range {
	
	int begin_;
	int end_;

	public:
		Range() = default;
		Range(int b = 0, int e = 0):begin_(b), end_(e) {}
		
		Range(const Range& r):begin_(r.begin_), end_(r.end_){}

		Range& operator = (const Range& r) {
			begin_ = r.begin_;
			end_   = r.end_;
			return *this;
		}

		Range(Range&& r):begin_(r.begin_), end_(r.end_) { 
			r.begin_ = 0;
			r.end_   = 0;
		}

		Range& operator = (Range&& r) { 
			this->begin_ = r.begin_;
			this->end_   = r.end_;
			
			r.begin_ = 0;
			r.end_ = 0;

			return *this;
		}

		~Range() = default;

		int begin() {return begin_; }
		int end() {return end_; }
		int size() { return end_ - begin_; }
};


};

#endif
