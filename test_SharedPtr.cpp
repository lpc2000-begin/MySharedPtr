#include "SharedPtr.hpp"
#include <iostream>

// CRTP
struct Student : EnableSharedFromThis<Student> {
    const char* name;
    int age;

    explicit Student(const char* name_, int age_) : name(name_), age(age_) {
        std::cout << "Student ����\n";
    }

    Student(Student&&) = delete;

    void func() {
        //std::cout << (void*)shared_from_this().get() << '\n';      
    }

    ~Student() {
        std::cout << "Student ����\n";
    }
};

struct StudentDerived : Student {
    explicit StudentDerived(const char* name_, int age_) : Student(name_, age_) {
        std::cout << "StudentDerived ����\n";
    }

    ~StudentDerived() {
        std::cout << "StudentDerived ����\n";
    }
};

int main() {
    SharedPtr<Student> p0(new StudentDerived("���ڱ�", 23));
    auto dp = staticPointerCast<StudentDerived>(p0);
    SharedPtr<Student const> bp = p0;
    p0 = constPointerCast<Student>(bp);
    std::cout << dp->name << '\n';

    SharedPtr<Student> p = makeShared<Student>("���ڱ�", 23); // make_shared һ���Դ���
    SharedPtr<Student> p2(new Student("���ڱ�", 23));         // �� new ����ָ����ٹ��죨���Ƽ���
    SharedPtr<Student> p3(new Student("���ڱ�", 23), [](Student* p) { delete p; });
    Student* raw_p = p.get();        // ��ȡԭʼָ��
    SharedPtr<Student> p4 = p;  // ǳ����
    SharedPtr<Student> p5 = p3; // ǳ����

    p5->func();

    p3 = p5;

    std::cout << p->name << ", " << p->age << '\n';
    std::cout << raw_p->name << ", " << raw_p->age << '\n';
}
