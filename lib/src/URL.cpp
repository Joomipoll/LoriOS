#include <Lori/Core/URL.h>
#include <Lori/Core/Lexer.h>

namespace Lori
{
    URL::URL(const char* url)
    {
        enum ParseState
        {
            Scheme,
            Authority,
            Port,
            Path,
            Query,
        };

        BasicLexer lex(url);
        ParseState pState = Scheme;

        while (!lex.End())
        {
            if (pState == Scheme)
            {
                std::string_view scheme = lex.EatWhile(isalnum);

                if (lex.End())
                {
                    pState = ParseState::Authority;
                    lex.Restart();
                    continue;
                }

                char c = lex.Eat();
                switch (c)
                {
                    case ':':
                        if (lex.Peek() == '/')
                        {
                            protocol = scheme;

                            lex.EatOne('/');
                            lex.EatOne('/'); // Consume up to two slashes

                            break;
                        }
                    case '@':
                    case '.':
                        pState = Authority;

                        lex.Restart();
                        continue;
                    case '/':
                        pState = Path;

                        lex.Restart();
                        continue;
                    default:
                        break;
                }

                pState = Authority;
            } else if (pState == Authority) {
                std::string_view auth = lex.EatWhile([](int c) -> int { return isalnum(c) || c == '.'; });

                if (lex.End())
                {
                    host = auth;
                    break;
                }

                char c = lex.Eat();
                switch (c)
                {
                    case '@':
                        userinfo = auth;
                        break;
                    case ':':
                        host = auth;

                        pState = Port;
                        break;
                    case '/':
                        host = auth;

                        pState = Path;
                        break;
                    default:
                        return; // Unknown character
                }
            } else if (pState == Port) {
                std::string_view auth = lex.EatWhile(isdigit);

                if (lex.End())
                {
                    port = auth;
                    break;
                }

                char c = lex.Eat();
                switch (c)
                {
                    case '/':
                        pState = Path;

                        port = auth;
                        break;
                    default:
                        return;
                }
            } else if (pState == Path){
                resource = lex.EatWhile([](int) -> int { return true; });
                break;
            }
        }

        valid = true;
    }
}
