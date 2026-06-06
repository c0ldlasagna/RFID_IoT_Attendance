<script lang="ts">
    interface Column {
        key: string;
        label: string;
    }

    interface Props {
        columns: Column[];
        rows: Record<string, any>[];
    }

    let { columns, rows }: Props = $props();
</script>

<div class="h-full overflow-y-auto ring ring-purple-500 bg-gray-950 rounded-md font-display">
    <table class="text-white grow w-full ">
        <thead class="sticky top-0 bg-accent">
            <tr>
                {#each columns as column}
                    <th class="p-3 text-left h-10">
                        {column.label}
                    </th>
                {/each}
            </tr>
        </thead>
        <tbody>
            {#each rows as row}
                <tr class="even:bg-accent">
                    {#each columns as column}
                      <td class="p-1 text-left pl-3 text-sm">
                        {column.label == "Time" || column.label=="Start Time" || column.label=="End Time"
                            ? row[column.key] ? new Date(row[column.key]).toLocaleString("id-ID") : null
                            : row[column.key]}
                      </td>
                    {/each}
                </tr>
            {/each}
        </tbody>
    </table>
</div>