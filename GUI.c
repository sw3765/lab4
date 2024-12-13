#include <QApplication>
#include <QPushButton>
#include <QMessageBox>

// 버튼 클릭 시 실행되는 함수
void onButtonClick() {
    QMessageBox::information(nullptr, "안내", "버튼이 클릭되었습니다!");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 버튼 생성
    QPushButton button("클릭하세요");
    button.resize(200, 100);

    // 버튼 클릭 시 이벤트 연결
    QObject::connect(&button, &QPushButton::clicked, &onButtonClick);

    button.show(); // 버튼 표시
    return app.exec(); // 이벤트 루프 실행
}
