
#include <kwgpu/karia.h>

int main()
{
    Karia k = Karia();

    k.Start();
    while (k.is_running)
    {
        k.Update();
        k.Draw();
    }
    k.End();
}
