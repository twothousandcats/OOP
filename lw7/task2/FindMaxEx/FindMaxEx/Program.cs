using FindMaxEx.Domain;

namespace FindMaxEx;

public class Program
{
    public static void Main()
    {
        IReadOnlyList<IAthlete> athletes = new List<IAthlete>
        {
            new Athlete( "Ivanov I.I.", 180, 75 ),
            new Athlete( "Petrov E.E.", 200, 90 ),
            new Athlete( "Sidorov A.A.", 188, 102 ),
        };

        if ( athletes.TryFindMax( ( a, b ) => a.Height < b.Height, out IAthlete tallestAthlete ) )
        {
            Console.WriteLine( $"Tallest athlete is {tallestAthlete.FullName}." );
        }

        if ( athletes.TryFindMax( ( a, b ) => a.Weight < b.Weight, out IAthlete heaviestAthlete ) )
        {
            Console.WriteLine( $"Heaviest athlete is {heaviestAthlete.FullName}." );
        }
    }
}