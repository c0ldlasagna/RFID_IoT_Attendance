import { SupabaseClient } from "@supabase/supabase-js";

export type SessionState =  | "active"
                            | "late"
                            | "idle"
                            | "registering";

export const session_state_map: Record<SessionState,number> = {
    "active" : 0,
    "late" : 1,
    "registering": 2,
    "idle" : 3
};

export interface Session{
    "session_id":number|null,
    "session_status":SessionState,
    "session_status_code":number,
    "register_name" : string|null|undefined
};

export const get_current_session = async(supabase:SupabaseClient): Promise<Session> => {
    const {data,error} = await supabase.from("sessions").select().is("end_time",null);
    let session_status : SessionState;
    let session_id : number|null = null;
    let register_name : string|null = null;
    
    if (data?.length == 1){
        session_status = data[0]["status"];
        session_id = data[0]["id"];
        register_name = data[0]["register_name"];
    }
    
    else{
        session_status = "idle";
    }
    
    return {
        "session_id" : session_id ?? null,
        "session_status" : session_status,
        "session_status_code" : session_state_map[session_status],
        "register_name": register_name
    }
}