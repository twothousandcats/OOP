using FindMaxEx.Domain;
using NUnit.Framework;

namespace FindMaxEx.Tests;

[TestFixture]
public class TryFindMaxTests
{
    // empty collection
    [Test]
    public void TryFindMax_EmptyCollection_ReturnsFalse()
    {
        IReadOnlyList<int> source = Array.Empty<int>();

        bool found = source.TryFindMax( ( a, b ) => a < b, out _ );

        Assert.That( found, Is.False );
    }

    // singleton
    [Test]
    public void TryFindMax_SingleElement_ReturnsThatElement()
    {
        IReadOnlyList<int> source = new[] { 1 };

        bool found = source.TryFindMax( ( a, b ) => a == b, out int max );

        Assert.That( found, Is.True );
        Assert.That( max, Is.EqualTo( 1 ) );
    }

    // comparator works correct
    [Test]
    public void TryFindMax_ReverseComparer_FindMinByInvertedPredicate()
    {
        IReadOnlyList<int> source = new[] { 3, 1, 4, 1, 5, 9, 2 };

        bool found = source.TryFindMax( ( a, b ) => a > b, out int min );

        Assert.That( found, Is.True );
        Assert.That( min, Is.EqualTo( 1 ) );
    }

    // nullSource -> throws
    [Test]
    public void TryFindMax_NullSource_ThrowsArgumentNullException()
    {
        IReadOnlyList<int>? source = null;

        Assert.Throws<ArgumentNullException>( () => source!.TryFindMax( ( a, b ) => a > b, out int max ) );
    }

    // nullPredicate -> throws
    [Test]
    public void TryFindMax_NullPredicate_ThrowsArgumentNullException()
    {
        IReadOnlyList<int> source = new[] { 1, 2, 3 };

        Assert.Throws<ArgumentNullException>( () => source.TryFindMax( null!, out _ ) );
    }

    [Test]
    public void TryFindMax_PredicateThrows_SourceIsUnchanged()
    {
        IReadOnlyList<int> source = new List<int>
        {
            1,
            2,
            3,
        }.AsReadOnly();

        IReadOnlyList<int> snapshot = source.ToArray();

        Assert.Throws<InvalidOperationException>( () =>
            source.TryFindMax(
                ( a, b ) => throw new InvalidOperationException(),
                out _
            )
        );

        Assert.That( source, Is.EqualTo( snapshot ) );
    }

    [Test]
    public void TryFindMax_EmptyCollection_OutIsDefault()
    {
        IReadOnlyList<int> src = Array.Empty<int>();

        src.TryFindMax( ( a, b ) => a < b, out int result );

        Assert.That( result, Is.EqualTo( default( int ) ) ); // 0
    }

    [Test]
    public void TryFindMax_AlwaysTruePredicate_ReturnsLastElement()
    {
        IReadOnlyList<int> src = new[] { 1, 2, 3 };

        src.TryFindMax( ( a, b ) => true, out int m );

        Assert.That( m, Is.EqualTo( 3 ) );
    }

    [Test]
    public void TryFindMax_AlwaysFalsePredicate_ReturnsFirstElement()
    {
        IReadOnlyList<int> src = new List<int>
        {
            1,
            2,
            3
        };

        src.TryFindMax( ( a, b ) => false, out int m );

        Assert.That( m, Is.EqualTo( 1 ) );
    }

    // Custom
    [Test]
    public void TryFindMax_WorksWithCustomClass_ReturnsCorrectResult()
    {
        IReadOnlyList<IAthlete> athletes = new List<IAthlete>
        {
            new Athlete( "Ivanov I.I.", 180, 75 ),
            new Athlete( "Petrov E.E.", 200, 90 ),
            new Athlete( "Sidorov A.A.", 188, 102 ),
        }.AsReadOnly();

        bool found = athletes.TryFindMax( ( a, b ) => a.Height < b.Height, out IAthlete tallestAthlete );

        Assert.That( found, Is.True );
        Assert.That( tallestAthlete, Is.Not.Null );
        Assert.That( tallestAthlete.FullName, Does.StartWith( "Petrov" ) );
    }
}