import { fail, redirect, type Action, type Actions } from "@sveltejs/kit";
import { type SessionState, session_state_map, type Session, get_current_session } from "$lib/session_status";
import type { PageServerLoad } from "../$types";

export const load:PageServerLoad = async({locals:{supabase}}) => {
    const {data:claims,error:claims_error} =await supabase.auth.getClaims();
    if( claims_error || !claims ){
        throw redirect(303,"/");
    }
    const {data:scan_logs,error:scan_logs_error} = await supabase.from("scan_logs").select();
    const {data:registered_cards,error:registered_cards_error} = await supabase.from("registered_cards").select();
    const current_session:Session = await get_current_session(supabase);
    const {data:sessions,error:sessions_error} = await supabase.from("sessions").select();
    return{
        current_session:current_session,
        sessions : sessions,
        scan_logs : scan_logs,
        registered_cards: registered_cards,
        register_name : current_session.register_name
    }
}

export const actions = {
    logout : async({locals:{supabase}})=>{
        await supabase.auth.signOut();
        redirect(303,"/");
    },
    state : async({request,locals:{supabase}})=>{
        const form_data:FormData = await request.formData();
        const status = form_data.get("session_status");
        const register_name = status=="registering" ? form_data.get("register_name") : null;

        if (status == "registering" && !register_name){
            return fail(400,{no_name:true});
        }

        const current_session:Session = await get_current_session(supabase);
        if(current_session.session_id){
            const {error} = await supabase.from("sessions").update({"end_time": new Date().toISOString()}).eq("id",current_session.session_id);
            if(error){
                console.log(error);
                return Response.json(
                    {
                        "message":"Failed to close previous session"
                    },
                    {
                        status:500
                    }
                )
            }
        }

        await supabase.from("sessions").insert({"status":status,"register_name": register_name });
    }
}satisfies Actions