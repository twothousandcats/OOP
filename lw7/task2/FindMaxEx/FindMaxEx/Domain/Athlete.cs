namespace FindMaxEx.Domain;

public class Athlete : IAthlete
{
    public Athlete( string name, int height, int weight )
    {
        FullName = name;
        Height = height;
        Weight = weight;
    }

    public string FullName { get; } // _fullName

    public int Height { get; } // _centimeter

    public int Weight { get; } // _kilograms
}