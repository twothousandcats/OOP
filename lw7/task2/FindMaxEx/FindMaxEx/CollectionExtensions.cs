namespace FindMaxEx;

public static class CollectionExtensions
{
    public static bool TryFindMax<TElement>(
        this IReadOnlyList<TElement> source,
        Func<TElement, TElement, bool> less,
        out TElement result
    ) where TElement : notnull
    {
        ArgumentNullException.ThrowIfNull( source );
        ArgumentNullException.ThrowIfNull( less );

        if ( source.Count == 0 )
        {
            result = default!;
            return false;
        }

        TElement element = source[ 0 ];
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