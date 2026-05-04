namespace FindMaxEx.Domain;

public class Athlete : IAthlete
{
    private readonly string _fullName;

    private readonly int _height;

    private readonly int _weight;

    public Athlete( string name, int height, int weight )
    {
        _fullName = name;
        _height = height;
        _weight = weight;
    }

    public string FullName { get; } // fullName

    public int Height { get; } // centimeter

    public int Weight { get; } // kilograms
}