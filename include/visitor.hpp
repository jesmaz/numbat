#ifndef VISITOR_HPP
#define VISITOR_HPP


#define VISITABLE public: virtual void accept (visitor::BaseVisitor & vis) {acceptVisitor (*this, vis);}

namespace numbat {
namespace visitor {


class BaseVisitor {
	public:
		virtual ~BaseVisitor () {}
};

template <typename T>
class Visitor : public virtual BaseVisitor {
	public:
		virtual void visit (T & target)=0;
		virtual ~Visitor () {}
};

class BaseVisitable {
	public:
		virtual void accept (BaseVisitor & vis) {};
		virtual ~BaseVisitable () {}
	protected:
		template <typename T>
		static void acceptVisitor (T & target, BaseVisitor & vis) {
			dynamic_cast <Visitor <T> &> (vis).visit (target);
		}
};


};
};


#endif