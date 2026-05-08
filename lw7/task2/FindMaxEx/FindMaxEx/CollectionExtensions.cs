namespace FindMaxEx;

public static class CollectionExtensions
{
    public static bool TryFindMax<T>(
        this IReadOnlyList<T> source,
        Func<T, T, bool> less,
        out T result
    ) where T : notnull
    {
        ArgumentNullException.ThrowIfNull( source );
        ArgumentNullException.ThrowIfNull( less );

        if ( source.Count == 0 )
        {
            result = default!;
            return false;
        }

        T element = source[ 0 ];
        for ( int i = 1; i < source.Count; i++ )
        {
            if ( less( element, source[ i ] ) )
            {
                element = source[ i ];
            }
        }

        result = element;
        return true;
    }
}