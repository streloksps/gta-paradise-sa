http://hashlib2plus.sourceforge.net/

#include <memory>
#include "hashlib++/hl_sha1wrapper.h"

void test() {
    std::tr1::shared_ptr<hashwrapper> sha1(new sha1wrapper());
    std::string rezult = sha1->getHashFromString("*|dimonml");
}
