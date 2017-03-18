#include "crow.h"
#include "fovris/Program.h"
#include "fovris/QlDeserializer.h"
#include "fovris/QueryResultPrinter.h"

#include <ostream>
#include <string>

std::string makeErrorMessage(const fovris::ParsingError &e) {
    return "Error at line  " + std::to_string(e.getLine()) + " ,position " +
           std::to_string(e.getPos());
}

std::string makeQueryErrorMessage(const fovris::InvalidInputException &e,
                                  const std::string &query) {
    return "Error in query: " + query + " . Reason: " + e.what();
}

void printHelp() {
    std::cout << "Usage: fovrisd [ADDR] [PORT]\n";
    std::cout << "Minimal stateless server that accepts 4QL scripts\n";
    std::cout << "and responds with results.\n\n";
    std::cout << "How to use:\n";
    std::cout << "Send POST request to the /query endpoint with 4QL\n";
    std::cout << "script in the body.\n";
}

int main(int argc, char **argv) {

    if (argc != 3 && argc != 1) {
        printHelp();
        return 1;
    }

    std::string addr = "0.0.0.0";
    int port = 8080;

    if (argc == 3) {
        addr = std::string(argv[1]);
        port = std::stoi(argv[2]);
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/query")
        .methods("POST"_method)([](const crow::request &req) {
            fovris::QlDeserializer sp;
            try {
                sp.parse(req.body);
            } catch (fovris::ParsingError &e) {
                return crow::response(400, makeErrorMessage(e));
            }
            fovris::Program program(fovris::Algorithm::Seminaive);
            for (auto &module : sp.getModules()) {
                program.addModule(module);
            }
            program.evaluate();
            crow::response resp;

            for (auto &query : sp.getQueries()) {
                std::ostringstream ss;
                try {
                    std::string resultStr = fovris::print(
                        fovris::QueryResultPrinter::Ql(program.query(query)));
                    resp.write(resultStr);
                } catch (fovris::InvalidInputException &e) {
                    return crow::response(
                        400, makeQueryErrorMessage(e, fovris::print(query)));
                }
                resp.write("\n");
            }

            if (sp.getQueries().empty()) {
                resp.write("Result is empty.");
            }

            return resp;
        });

    CROW_LOG_INFO << "Starting server: " << addr << ":" << port;
    app.bindaddr(addr).port(port).multithreaded().run();
}
