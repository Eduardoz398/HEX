#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

/**
 * 
 * compilar program
 * g++ -o myapp myapp.cpp -std=c++17 -I/opt/local/include -L/opt/local/lib -lwt -lwthttp -Wl,-rpath,/opt/local/lib
 * 
 * compiler and run program
 * g++ -o myapp myapp.cpp -std=c++17 -I/opt/local/include -L/opt/local/lib -lwt -lwthttp -Wl,-rpath,/opt/local/lib && ./myapp --docroot . --http-address 0.0.0.0 --http-port 8080
 */

class MyApp : public Wt::WApplication {
public:
    MyApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
        setTitle("Interação com C++");

        useStyleSheet(Wt::WLink("https://cdn.jsdelivr.net/npm/tailwindcss@2.2.19/dist/tailwind.min.css"));

        auto container = root();
        auto text = container->addWidget(std::make_unique<Wt::WText>("Contador: 0"));
        auto button = container->addWidget(std::make_unique<Wt::WPushButton>("Aumentar"));

        container->addStyleClass("p-4");
        text->addStyleClass("text-lg text-blue-500 pr-2");
        button->addStyleClass("bg-blue-500 text-white p-2 rounded");
        
        int* count = new int(0);
        
        button->clicked().connect([=]() mutable {
            (*count)++;
            text->setText("Contador: " + std::to_string(*count));
        });
    }
};

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<MyApp>(env);
}

int main(int argc, char **argv) {
    return Wt::WRun(argc, argv, &createApplication);
}