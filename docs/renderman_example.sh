surface basicSpecular(
    color myOpacity = 1; 
    float roughness = 0.1;
)
{
        color myColor = (1.0, 0.0, 0.0);
        normal Nn = normalize(N);
        //Specular stuff
        vector V = normalize(-I);
        
        Ci = myColor * myOpacity * diffuse(Nn) + specular(Nn, V, roughness);
        Oi = myOpacity;
} 