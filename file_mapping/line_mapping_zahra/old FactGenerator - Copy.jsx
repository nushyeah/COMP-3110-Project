import { ThreeDot, OrbitProgress  } from "react-loading-indicators"
import { useEffect, useState } from "react"

function FactGenerator(){

    const [fact, setFact] = useState('')
    const [loading, setLoading] = useState(false)

    const getFact = async function(){
        if (loading){
            console.log("still loading")
            return
        }
        try {
            setLoading(true)
            const response = await fetch ("https://uselessfacts.jsph.pl/api/v2/facts/random?language=en")
            let json = await response.json()
    
            console.log(json)
            setFact(json.text)
            setLoading(false)
        }
        catch (e) {
            console.log(e)
            setLoading(false)
        }
    }

    useEffect(() => {
        console.log("is mounted")
        getFact()
    }, 
    [])

    return (

        <div>
            <h1 className="text-white font-bold mb-10" > Fact Generator</h1>

            <div className="m-14">
                
                {loading && <OrbitProgress variant="spokes" dense color="white" size="medium" text="" textColor="" />
                ||

                <p className="text-2xl text-white font-bold">
                {fact}<br/>
                </p>}

            </div>
            

            <button className="font-bold bg-gray-900 p-3 rounded-xl cursor-pointer 
             hover:outline-white hover:outline-2
             hover:scale-105 hover:bg-gray-800
             duration-75 ease-in"
            onClick={() => getFact()}>
                Generate Fact
            </button>

        </div>
    )
}

export default FactGenerator