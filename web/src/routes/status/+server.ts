import type { RequestHandler,RequestEvent } from "@sveltejs/kit";
import { type Session } from "$lib/session_status";
import { get_current_session } from "$lib/session_status";

export const GET:RequestHandler = async({locals:{supabase}}) => { 
    const response_data:Session =  await get_current_session(supabase);
    return Response.json(
        response_data,
        {
            status:200
        }
    )
};